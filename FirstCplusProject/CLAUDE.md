# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

- **Engine:** Unreal Engine **5.7** (`EngineAssociation` in `FirstCplusProject.uproject`).
- **Type:** C++ Game module (`Source/FirstCplusProject.Target.cs` → `TargetType.Game`).
- **Module name:** `FirstCplusProject` (single runtime module). API macro: `FIRSTCPLUSPROJECT_API`.
- **Plugins enabled:** `EnhancedInput` (runtime), `ModelingToolsEditorMode` (editor-only).
- **Public deps** (`FirstCplusProject.Build.cs`): `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `Niagara`, `UMG`, `AIModule`, `AnimationCore`, `AnimGraphRuntime`. **Private:** `Slate`, `SlateCore`. Add new deps here when including their headers.
- **Build settings:** `BuildSettingsVersion.V5`, `IncludeOrderVersion.Latest`, `PCHUsage = UseExplicitOrSharedPCHs` — explicit includes are required (no monolithic PCH).

## Building and Running

This is a Windows project (`.sln`/`.vs/`/Visual Studio + UE 5.7). There is **no `dotnet`/`make`/`cmake`** layer — UnrealBuildTool drives everything.

- **Regenerate project files** (after adding/removing `.cpp`/`.h`): right-click `FirstCplusProject.uproject` → *Generate Visual Studio project files*, or:
  ```
  "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe" -projectfiles -project="<abs path>\FirstCplusProject.uproject" -game -rocket -progress
  ```
- **Build (editor target)** from CLI:
  ```
  "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat" FirstCplusProjectEditor Win64 Development -Project="<abs path>\FirstCplusProject.uproject" -WaitMutex -FromMsBuild
  ```
- **Build (game target):** replace `FirstCplusProjectEditor` with `FirstCplusProject`.
- **Run editor:** open `FirstCplusProject.uproject` (double-click) or `UnrealEditor.exe "<abs path>\FirstCplusProject.uproject"`.
- **Hot Reload / Live Coding:** use Live Coding from inside the editor (Ctrl+Alt+F11) for iterative C++ changes. Header-layout changes (new `UPROPERTY`, new fields) require a full editor restart and rebuild.
- **Tests:** the project has **no Automation Test specs** (no `*Tests` module). Don't claim test runs you didn't do — verify changes by building both targets and exercising the gameplay path in PIE.

## Source Layout

```
Source/
  FirstCplusProject.Target.cs           # game target
  FirstCplusProjectEditor.Target.cs     # editor target
  FirstCplusProject/
    FirstCplusProject.Build.cs          # module deps (edit when adding includes)
    FirstCplusProject.{h,cpp}           # module impl (IMPLEMENT_PRIMARY_GAME_MODULE)
    MyObject.{h,cpp}                    # plain UObject example
    GameplayActors/                     # all gameplay code lives here
```

Every `.h` defining a `UCLASS`/`USTRUCT`/`UENUM` must end with `#include "<Name>.generated.h"` as its **last** include — Unreal Header Tool regenerates these on build. Don't hand-edit generated files under `Intermediate/`.

`Binaries/`, `DerivedDataCache/`, `Intermediate/`, `Saved/` are build/runtime artifacts (see `.gitignore`) — never commit.

## Architecture

A single-player action game with a player character, AI enemies, weapon pickups, and exploration props. Everything important is under `Source/FirstCplusProject/GameplayActors/`.

### Player

- **`AMainCharacter`** (`ACharacter`) — protagonist. Owns:
  - **Camera rig:** `USpringArmComponent CameraBoom` + `UCameraComponent FollowCamera`, controller-rotation driven on the boom only.
  - **Stats:** `Health/MaxHealth`, `Stamina/MaxStamina`, `Coins/MaxCoins`, plus enums `EMovementStatus` (Normal/Sprinting) and `EStaminaStatus` (Normal/BelowMinimum/Exhausted/ExhaustedRecovering). Sprint drains stamina at `StaminaDrainRate`; `MinSprintStamina` gates re-entry into sprint after exhaustion. Speed switches between `RunningSpeed` and `SprintingSpeed` via `UpdateMovementSpeed()`.
  - **Combat:** holds `EquippedWeapon` (`AWeapon*`) and `ActiveOverlappingItem` (`AItem*`). `Attack()` plays `CombatMontage`; `AttackEnd()` is `BlueprintCallable` (called from anim notify) and is also backed by a `FTimerHandle AttackTimer` safety in case the notify is missing. `bAttacking`/`bLMB` flags coordinate with the AnimInstance.
  - **Death:** `Die()` sets `bIsDead`, used to gate input/movement.
- **Input is Enhanced Input.** All actions are `UInputAction*` `UPROPERTY`s set in the BP defaults (Move, Look, TurnRate, LookUpRate, StartJumping/StopJumping, StartSprinting/StopSprinting, LMBDown/LMBUp), bound through `UEnhancedInputComponent` in `SetupPlayerInputComponent`. The mapping context (`UInputMappingContext* InputMapping`) is asset-set in BP and lives at `Content/Input/MainInputMappingContext.uasset`. Don't add legacy `BindAxis`/`BindAction` calls.
- **`AMainPlayerController`** spawns the HUD UMG widget (`HUDOverlayAsset` → `HUDOverlay`) on `BeginPlay`.
- **`AMyGameModeBase`** is an empty `AGameModeBase` placeholder — the actual default pawn/HUD/controller wiring is configured in `Config/DefaultEngine.ini` and the BP subclass.

### Enemies and AI

- **`AEnemy`** (`ACharacter`) drives all hostile NPCs. Detection/combat is **sphere-based**, not perception-component based:
  - **`AgroSphere`** (`USphereComponent`) — when the player overlaps, transition to `EMS_MoveToTarget` and the cached `AAIController* AIController` runs `MoveToTarget(MainCharacter)`. On overlap-end, return to idle.
  - **`CombatSphere`** (smaller `USphereComponent`) — when the player overlaps, transition to `EMS_Attacking` and call `Attack()` which plays `CombatMontage` and rotates toward the player before the swing.
  - **`CombatCollision`** (`UBoxComponent` on the weapon socket) is **disabled by default** and toggled by `ActivateCollision()`/`DeactivateCollision()` from anim notifies during the swing window. Hits land via its overlap delegates (`CombatOnOverlapBegin`/`End`).
  - State machine: `EEnemyMovementStatus` (Idle / MoveToTarget / Attacking) gated by `SetEnemyMovementStatus`. `bAttackEndHandled` prevents double-fire when both the anim notify and `OnAttackMontageEnded` deliver `AttackEnd`.

### Items, Weapons, Pickups

- **`AItem`** is the base class: `USphereComponent CollisionVolume`, `UStaticMeshComponent Mesh`, optional `UNiagaraComponent IdleParticlesComponent` + `UNiagaraSystem* OverlapParticles`, `USoundCue* OverlapSound`, idle rotation toggle. Overlap delegates are virtual — subclasses override.
- **`AWeapon : AItem`** adds `EWeaponState` (Pickup/Equipped), `UBoxComponent CombatCollision`, equip/swing sounds, and `Equip(AMainCharacter*)` to attach to the character's `RightHandSocket`. Same `ActivateCollision`/`DeactivateCollision` pattern as `AEnemy` — anim notifies on the player's `CombatMontage` toggle the weapon's hit volume.
- **`APickup : AItem`** — coin/health/etc. consumables.
- **`AExplosive : AItem`** — damage-on-overlap variant.
- Floor/movement props: **`AFloor​Switch`**, **`AFloater`**, **`AFloatingPlatform`**, **`ASpawnVolume`**.
- **`ACritter`** (passive pawn) and **`ACollider`** + **`UColliderMovementComponent`** are standalone learning examples — not part of the main combat loop.

### Animation

- Two `UAnimInstance` subclasses: **`UMainCharacterAnimInstance`** and **`UEnemyAnimInstance`**. Both follow the same pattern:
  - `NativeInitializeAnimation()` caches the owning `APawn` / `AMainCharacter` / `AEnemy`.
  - `UpdateAnimationProperties()` is `BlueprintCallable` and pushes `MovementSpeed`, `Direction`, `bIsInAir` (and similar) into the AnimBlueprint each tick. Both pointers may be null between possession events — every read in these methods must null-check.
  - `MainCharacterAnimInstance` exposes `UBlendSpace* LocomotionBlendSpace` set in the AnimBP class defaults.
- Attack timing is montage-driven: notifies on `CombatMontage` call `ActivateCollision`/`DeactivateCollision` (start/end of swing) and `AttackEnd` (on the `End` notify). When adding a new attack, wire the same three notifies or the hit window will never open/close.

### Cross-cutting conventions

- **Forward-declare in headers, include in `.cpp`** — most headers in this project already do this (`class UParticleSystem;`, `class USoundCue;`, etc.). Keep it that way to avoid ballooning recompile times.
- **`UPROPERTY` reflects to BP** — anything tunable from designers should be `EditAnywhere` + `BlueprintReadWrite`; runtime-only state is `VisibleAnywhere` + `BlueprintReadOnly`. `EditDefaultsOnly` is used for input-action and montage asset slots so they're class-default-only.
- **Null-check UObject pointers before dereferencing** in Tick paths and overlap callbacks — pawns/controllers/anim instances can be unset transiently (level transitions, possession changes). The codebase has been hardened against this in recent commits; preserve the pattern.
- **Sounds:** play via `UGameplayStatics::PlaySound2D` with `USoundCue` assets — the SoundCue assets are referenced as `UPROPERTY` slots and assigned in BP defaults.

## Working with Generated/Cached State

- After adding a new `UCLASS` or moving files: **regenerate project files** before building, or VS won't see them.
- If the editor refuses to load after a header change, delete `Binaries/` and `Intermediate/` for `FirstCplusProject` and rebuild from the IDE — UHT-stale artifacts are the usual cause.
- `Saved/Config/` holds per-user editor state — never commit. Engine-wide config lives in `Config/Default*.ini`.

## Other Files in the Repo

`CODE_FIX_PLAN.md`, `QUICK_FIX_CHECKLIST.md`, `README_FIX_PACKAGE.md`, `GIT_COMMIT_GUIDE.md`, `PROGRESS_SUMMARY.md` are artifacts from a one-off code-review pass (dated 2025-12-08). They are **not** project documentation — treat them as historical notes, not authoritative guidance. The current branch (`feat/combat`) has already addressed the crash fixes those documents describe.