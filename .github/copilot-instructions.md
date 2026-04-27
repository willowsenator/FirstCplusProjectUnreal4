# Copilot Instructions — FirstCplusProject

## Project at a glance

- **Engine:** Unreal Engine **5.7** (`FirstCplusProject.uproject`).
- **Type:** Single-player action game, C++ first.
- **Module:** one runtime game module, `FirstCplusProject`. API macro `FIRSTCPLUSPROJECT_API`. Module dependencies are declared in `Source/FirstCplusProject/FirstCplusProject.Build.cs` — anything new pulled into a header must be listed there.
- **Targets:** `FirstCplusProject` (game) and `FirstCplusProjectEditor`. Both inherit `BuildSettingsVersion.V5` and `IncludeOrderVersion.Latest`. PCH mode is `UseExplicitOrSharedPCHs` — every translation unit declares its own includes.
- **Plugins enabled:** `EnhancedInput` (runtime) and `ModelingToolsEditorMode` (editor-only).
- **Primary language:** C++ (Unreal dialect — UHT-generated reflection). Build/target scripts are C# (`.Build.cs`, `.Target.cs`).

## Source map

```
Source/
  FirstCplusProject/
    FirstCplusProject.{h,cpp,Build.cs}     # module entry + deps
    MyObject.{h,cpp}                       # plain UObject example
    GameplayActors/                        # all gameplay classes live here
```

`Binaries/`, `DerivedDataCache/`, `Intermediate/`, `Saved/` are build artifacts (see `.gitignore`); they are never reviewed.

## Architecture

The game is built around three actor families that interact through component overlaps and animation montages:

1. **Player (`AMainCharacter` extends `ACharacter`)** — owns a `USpringArmComponent` + `UCameraComponent` rig, an Enhanced Input mapping context, a stamina/health/coins stat block, and an equipped `AWeapon`. Sprint is governed by an `EStaminaStatus` state machine (`Normal` → `BelowMinimum` → `Exhausted` → `ExhaustedRecovering`); attack timing is driven by a `UAnimMontage` with anim-notify callbacks.
2. **Enemies (`AEnemy` extends `ACharacter`)** — drive AI via two nested `USphereComponent`s: an outer **agro sphere** that triggers `MoveToTarget` through a cached `AAIController`, and an inner **combat sphere** that triggers an attack montage. A `UBoxComponent` weapon collider is enabled only during the swing window. State is an `EEnemyMovementStatus` (`Idle` / `MoveToTarget` / `Attacking`).
3. **Items (`AItem` and subclasses `AWeapon`, `APickup`, `AExplosive`)** — share a `USphereComponent` overlap volume, an optional Niagara idle particle, and an overlap sound. `AWeapon` adds an `EWeaponState` (Pickup/Equipped) and a `UBoxComponent` hit volume that is toggled on by anim notifies during the swing.

Animation is bridged into C++ via two `UAnimInstance` subclasses (`UMainCharacterAnimInstance`, `UEnemyAnimInstance`). Both follow the same shape: `NativeInitializeAnimation` caches the owning pawn, and a `BlueprintCallable` update method pushes movement speed / direction / in-air flags into the AnimBP each tick.

The HUD is a UMG widget owned by `AMainPlayerController` (`HUDOverlayAsset` → `HUDOverlay`). `AMyGameModeBase` is an empty `AGameModeBase` subclass; default pawn / controller / HUD are wired in `Config/DefaultEngine.ini` and the BP subclass.

## Safety guards / domain invariants

- **UObject pointers can be null transiently** during level transitions, possession changes, and montage interruptions. Every read of a cached pointer in `Tick`, overlap delegates, and `UAnimInstance::UpdateAnimationProperties` must null-check before dereferencing.
- **Combat hit windows are anim-driven.** Both player weapon and enemy weapon colliders default to `NoCollision` and are flipped on/off only by anim notifies (`ActivateCollision` / `DeactivateCollision` / `AttackEnd`). Code paths that toggle these volumes outside montage callbacks are bugs.
- **Double-fire guard on attack end.** `AEnemy::AttackEnd` is reachable from both the anim notify and `OnAttackMontageEnded`; a `bAttackEndHandled` flag prevents double-processing. `AMainCharacter` uses an `FTimerHandle AttackTimer` as a missing-notify safety net — a swing must always end exactly once.
- **`Die()` is one-shot.** `AMainCharacter::bIsDead` gates input/movement; re-entry into death paths is by design rejected.
- **Stamina sprint re-entry** requires stamina ≥ `MinSprintStamina`, not just > 0. The hysteresis is intentional to prevent rapid sprint flicker.

## Input model

- **All input is Enhanced Input.** Action slots (`UInputAction*`) are class-default `UPROPERTY`s set on the BP; the mapping context (`UInputMappingContext*`) is asset-set and lives at `Content/Input/MainInputMappingContext.uasset`. Bindings use `UEnhancedInputComponent`.
- Legacy `BindAxis`/`BindAction` calls are **not** used and should not be reintroduced.

## Concurrency model

- Gameplay code runs on the game thread. There is no custom threading, no `Async` task graph usage, and no `FRunnable` workers in this project.
- Time-deferred work uses `FTimerManager` via `FTimerHandle`s on the owning actor.
- Niagara/animation update happens on engine-managed worker threads — gameplay code only interacts with them through the standard component APIs.

## Test coverage

**There is no Automation Test suite in this project.** No `*Tests` module, no `.spec.cpp` files, no Functional Tests under `Content/`. Verification is manual through PIE / standalone game runs. Do not flag missing unit tests for individual gameplay classes; flag the absence only when a `*Tests` module is added and gaps appear there.

## Reflection / generated headers

- Every `.h` declaring a `UCLASS`, `USTRUCT`, or `UENUM` ends with `#include "<Name>.generated.h"` as the last include. UHT regenerates these on build.
- `UPROPERTY` access tiers in this codebase follow a deliberate pattern:
  - `EditAnywhere, BlueprintReadWrite` — designer-tunable runtime values (stats, speeds, rates).
  - `VisibleAnywhere, BlueprintReadOnly` — runtime state surfaced to BP for read-only display (movement status, equipped weapon).
  - `EditDefaultsOnly, BlueprintReadOnly` — class-default-only asset slots (input actions, montages, HUD class).

## Intentional patterns — do NOT flag

- **`FORCEINLINE` one-line setters/getters** (e.g., `SetEquippedWeapon`, `GetWeaponState`) on hot-path actors — intentional, not "missing implementation file".
- **Forward declarations in headers** (`class UParticleSystem;`, `class USoundCue;`) with the matching `#include` only in the `.cpp` — required by the explicit-PCH build mode, not a missing include.
- **Cached raw `UPROPERTY` pointers** (e.g., `AAIController* AIController`, `AMainCharacter* CombatTarget`) — `UPROPERTY` participates in GC, so these are not raw-pointer leaks.
- **Sphere components for AI detection** instead of `UAIPerceptionComponent` — deliberate simplification; do not suggest swapping to perception components without a domain-level reason.
- **Empty `AMyGameModeBase`** — placeholder; behavior is configured in BP defaults and `DefaultEngine.ini`.
- **`MyObject` and `ACritter` / `ACollider` / `UColliderMovementComponent`** — standalone learning examples kept in tree; not part of the combat loop. Do not flag as dead code.
- **Top-level `*_FIX_*.md` / `PROGRESS_SUMMARY.md` / `README_FIX_PACKAGE.md`** — historical artifacts of a one-off code-review pass (2025-12-08). Not authoritative project docs.

## Build / iteration notes

- Header-shape changes (new `UPROPERTY`, new fields, new reflected types) require a full editor restart and rebuild — Live Coding alone is insufficient. Body-only changes are Live-Coding compatible.
- Adding a new `.cpp`/`.h` requires regenerating Visual Studio project files before the IDE will see them.
- New module dependencies must be added to `FirstCplusProject.Build.cs` `PublicDependencyModuleNames` (or `PrivateDependencyModuleNames` if the include is `.cpp`-only) — missing entries surface as cryptic linker errors.