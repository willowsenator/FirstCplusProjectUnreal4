---
applyTo: "**/*.cpp,**/*.h,**/*.hpp,**/*.cs,**/*.uproject,**/*.ini"
---

# Code Review Instructions — FirstCplusProject

Project context lives in `.github/copilot-instructions.md`. This file describes **how** to review changes; do not duplicate the architecture or "do not flag" lists from that file — cross-reference them.

## Architecture

- **Respect the actor-family boundaries.** Player (`ACharacter`), enemy AI (`ACharacter` + sphere triggers), and items (`AItem` + subclasses) interact through component overlaps and `UAnimMontage` notifies. Cross-cutting changes that bypass these boundaries (e.g., enemies poking player internals directly, items mutating player stats without going through the player API) are smells.
- **Animation drives combat timing, not `Tick`.** Activating/deactivating a hit volume, ending an attack, or applying damage on a swing must be reachable from anim-notify callbacks. Frame-counted or timer-only hit windows in `Tick` are a regression of the established pattern.
- **State machines stay in their owners.** Player movement/stamina enums live on `AMainCharacter`; enemy movement enum on `AEnemy`; weapon state on `AWeapon`. External code reads them through accessors — direct enum mutation from another actor is a layering violation.
- **Module dependencies are declared explicitly.** Any new `#include` that pulls in a module not already in `FirstCplusProject.Build.cs` must add that module to `PublicDependencyModuleNames` or `PrivateDependencyModuleNames`. Flag PRs that include a new engine subsystem header without updating `Build.cs`.

## Clean code

- **Function size:** combat / input / overlap callbacks should stay short. If a single overlap handler grows past ~40 lines or branches deeply on actor types, suggest extracting a typed helper.
- **Naming:** match Unreal conventions — `A`-prefix for actors, `U`-prefix for `UObject`, `F`-prefix for plain structs, `b`-prefix for booleans, `EWhatever` for `UENUM`s. PascalCase for everything reflected.
- **Dead code:** check the base branch before flagging — new feature scaffolding on an active branch is not dead. The standalone learning examples (`MyObject`, `ACritter`, `ACollider`, `UColliderMovementComponent`) are intentional; do not propose deleting them.
- **Comments:** prefer self-documenting code. Flag comments that restate the next line; keep comments that explain *why* a non-obvious branch exists (e.g., the double-fire guard, the missing-notify timer fallback).
- **No `TODO`/`FIXME` left without an owner or ticket reference.**

## Unreal C++ specifics

- **`UPROPERTY` lifetimes.** Raw `UObject*` fields holding references across frames must be `UPROPERTY` so GC can keep them alive. Bare `AActor*` / `UComponent*` members that survive past a single function are red flags.
- **`UPROPERTY` access tier.** Match the existing pattern:
  - Designer-tunable runtime values → `EditAnywhere, BlueprintReadWrite`.
  - Read-only runtime state → `VisibleAnywhere, BlueprintReadOnly`.
  - Class-default asset slots (input actions, montages, widget classes) → `EditDefaultsOnly, BlueprintReadOnly`.
- **Generated headers.** `#include "<Name>.generated.h"` must be the **last** include in any header that declares a reflected type. New reflected types without it will not compile under UHT.
- **Forward declare in `.h`, include in `.cpp`** — the project uses `PCHUsage = UseExplicitOrSharedPCHs`. Pulling a heavy engine header into another header for a single pointer parameter is a build-time regression.
- **Null-check transient pointers.** Cached pawn / controller / anim-instance pointers can become null between possession events. Every read of a cached pointer in `Tick`, overlap callbacks, and `UAnimInstance` update functions must null-check before dereferencing.
- **Avoid `static_cast`/`Cast<>` in hot paths without a check.** Use `Cast<T>()` (which returns nullptr on failure) and branch — never assume a cast succeeded.
- **`FORCEINLINE`** is reserved for trivial accessors. Flag use on functions with branches, allocations, or iteration.
- **Construction-time work.** Constructors should only set defaults and create components; gameplay state setup belongs in `BeginPlay`.
- **Hard-coded asset paths** (`ConstructorHelpers::FObjectFinder` with literal `/Game/...` strings) are discouraged — prefer `EditDefaultsOnly` properties set in BP.

## Input

- All new player input must be wired through Enhanced Input (`UInputAction*` slot + `UEnhancedInputComponent` binding). Flag any reintroduction of legacy `BindAxis` / `BindAction`.
- Mapping contexts are assets, not code-built — additions go through `MainInputMappingContext.uasset`, not `AddMappingContext` calls with literal keys.

## Animation

- New attacks must wire all three notifies: `ActivateCollision` (start of swing), `DeactivateCollision` (end of swing), `AttackEnd` (montage end). Missing any of them produces stuck-on hit volumes or a stuck `bAttacking` flag.
- `UAnimInstance::UpdateAnimationProperties` must guard every cached pointer; do not assume the owning pawn is set.

## Security & input validation

- This is a single-player offline game — no network, no user-supplied strings, no auth surface. Standard injection / XSS / SQLi rules do not apply.
- **Do** flag: secrets, API keys, or absolute filesystem paths checked into source or `Config/Default*.ini`; large binary blobs committed under `Source/`; `Saved/` or `Intermediate/` content checked in.
- **Do** flag: telemetry / network code added without an explicit feature request — the project has no network layer today, so adding one is an architecture-level change, not a routine PR.

## Testing

- The project has **no** Automation Test suite at present. Do not flag individual classes for missing unit tests.
- If a PR adds a `*Tests` module or `.spec.cpp` files, then apply standard expectations: new gameplay logic comes with a test, public APIs are exercised, mocks live at module boundaries.
- Manual verification (PIE run, gameplay scenario walked through) should be described in the PR body for behavioral changes.

## Performance

- **`Tick` is hot.** Object lookups (`GetWorld`, `FindComponentByClass`, `Cast<>` chains) inside `Tick` should be cached at `BeginPlay`. Allocations inside `Tick` (TArray growth, dynamic strings) need justification.
- **Sphere/box overlap callbacks fire frequently.** Avoid heavy per-overlap work — debug logs, asset loads, or `Cast<>` chains across multiple types are smells.
- **Per-frame `UE_LOG`** is a regression. Logs in `Tick` or overlap callbacks must be gated behind a verbosity check or removed before merge.
- **Niagara / particle / sound spawns** in tight loops without a cooldown are smells — flag these.

## Config / build hygiene

- `Config/Default*.ini` changes are reviewed: flag credentials, absolute paths, and editor-only settings leaking into runtime sections.
- `*.Build.cs` / `*.Target.cs` changes require a build-from-clean check; flag PRs that add deps without updating both targets when applicable.
- Do **not** commit `Binaries/`, `Intermediate/`, `DerivedDataCache/`, `Saved/`, `*.sln`, `*_BuiltData.uasset`, or `*.user` files. The `.gitignore` covers these — flag any PR that re-adds them.

## What is NOT a finding

See `.github/copilot-instructions.md` → *Intentional patterns — do NOT flag* for the canonical list. The most common false positives are: `FORCEINLINE` accessors, forward declarations with `.cpp`-only includes, `UPROPERTY` raw pointers, sphere components used in place of `UAIPerceptionComponent`, the empty `AMyGameModeBase`, and the standalone learning examples (`MyObject`, `ACritter`, `ACollider`).