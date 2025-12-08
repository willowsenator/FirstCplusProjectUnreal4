# Code Fix Implementation Plan
**Project:** FirstCplusProject (Unreal Engine C++)  
**Date:** 2025-12-08  
**Status:** Ready for Implementation

---

## 📋 Overview

This plan addresses all issues identified in the code review, organized by priority and dependency. Each step includes specific files to modify, exact changes needed, and validation criteria.

**Estimated Total Time:** 4-6 hours  
**Risk Level:** Medium (requires testing after each critical fix)

---

## 🔴 PHASE 1: Critical Fixes (MUST DO FIRST)
**Time Estimate:** 1-2 hours  
**Risk:** High - These cause crashes

### Step 1.1: Fix MainPlayerController Null Pointer Crash ⚡
**File:** `GameplayActors/MainPlayerController.cpp`  
**Line:** 16  
**Time:** 10 minutes

**Current Code:**
```cpp
void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (HUDOverlayAsset)
    {
        HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
    }

    HUDOverlay->AddToViewport();  // ❌ CRASHES IF NULL
    HUDOverlay->SetVisibility(ESlateVisibility::Visible);
}
```

**Fixed Code:**
```cpp
void AMainPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (HUDOverlayAsset)
    {
        HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);
    }

    if (HUDOverlay)  // ✅ SAFE
    {
        HUDOverlay->AddToViewport();
        HUDOverlay->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("HUDOverlay failed to create. Check HUDOverlayAsset."));
    }
}
```

**Validation:**
- [ ] Project compiles
- [ ] Game launches without crash when HUDOverlayAsset is not set
- [ ] HUD displays correctly when HUDOverlayAsset IS set

---

### Step 1.2: Fix Floater Variable Initialization Bug 🐛
**File:** `GameplayActors/Floater.cpp`  
**Line:** 22  
**Time:** 5 minutes

**Current Code:**
```cpp
A, B, C, D = 0.f;  // ❌ ONLY D IS SET TO 0!
```

**Fixed Code:**
```cpp
A = 0.f;
B = 0.f;
C = 0.f;
D = 0.f;
```

**Validation:**
- [ ] Project compiles
- [ ] Floater actors behave correctly with default values
- [ ] Sine wave motion works as expected

---

### Step 1.3: Fix MainCharacter Die() Function 💀
**Files:** 
- `GameplayActors/MainCharacter.h` (Line 154)
- `GameplayActors/MainCharacter.cpp`

**Time:** 20 minutes

**Current Header:**
```cpp
static void Die();  // ❌ STATIC - CAN'T ACCESS INSTANCE DATA
```

**Fixed Header:**
```cpp
void Die();

UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
bool bIsDead;
```

**Current Implementation:**
```cpp
void AMainCharacter::Die()
{
    UE_LOG(LogTemp, Warning, TEXT("Player died"));
}
```

**Fixed Implementation:**
```cpp
void AMainCharacter::Die()
{
    if (bIsDead) return;  // Prevent multiple deaths
    
    bIsDead = true;
    
    // Disable player input
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (PC)
    {
        DisableInput(PC);
    }
    
    // Stop movement
    GetCharacterMovement()->DisableMovement();
    
    // Disable collision
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // TODO: Play death animation
    // TODO: Trigger respawn timer
    // TODO: Notify GameMode
    
    UE_LOG(LogTemp, Warning, TEXT("Player died at location: %s"), *GetActorLocation().ToString());
}
```

**Also Initialize in Constructor:**
```cpp
// In AMainCharacter::AMainCharacter()
bIsDead = false;
bAttacking = false;
bSprinting = false;
bLMB = false;
```

**Validation:**
- [ ] Project compiles
- [ ] Die() can be called and properly disables character
- [ ] Multiple Die() calls don't cause issues
- [ ] bIsDead flag works correctly

---

### Step 1.4: Add Critical Null Checks in MainCharacter 🛡️
**File:** `GameplayActors/MainCharacter.cpp`  
**Lines:** 250 (LMBDown), 369 (Attack)  
**Time:** 15 minutes

**Fix LMBDown() - Line 250:**
```cpp
void AMainCharacter::LMBDown(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        bLMB = true;
        
        if (ActiveOverlappingItem)
        {
            AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
            if (Weapon)  // ✅ NULL CHECK
            {
                Weapon->Equip(this);
                ActiveOverlappingItem = nullptr;
            }
        }
        else if (EquippedWeapon)  // ✅ ALREADY HAS NULL CHECK
        {
            Attack();
        }
    }
}
```

**Fix Attack() - Line 369:**
```cpp
void AMainCharacter::Attack()
{
    if (!EquippedWeapon)  // ✅ ADD THIS CHECK
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot attack - no weapon equipped"));
        return;
    }
    
    if (bAttacking) return;

    bAttacking = true;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && CombatMontage)
    {
        switch (FMath::RandRange(0, 1))
        {
        case 0:
            AnimInstance->Montage_Play(CombatMontage, 2.2f);
            AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
            break;
        case 1:
            AnimInstance->Montage_Play(CombatMontage, 1.8f);
            AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
            break;
        default:
            break;
        }
    }
}
```

**Validation:**
- [ ] Project compiles
- [ ] No crash when attacking without weapon
- [ ] No crash when overlapping non-weapon items
- [ ] Proper error messages in log

---

## 🟡 PHASE 2: High Priority Fixes
**Time Estimate:** 1-2 hours  
**Risk:** Medium - Logic errors

### Step 2.1: Fix Enemy Combat Target Logic 🎯
**File:** `GameplayActors/Enemy.cpp`  
**Line:** 103-109  
**Time:** 10 minutes

**Current Code:**
```cpp
void AEnemy::CombatSphereOnOverlapEnd(...)
{
    if (OtherActor)
    {
        if (const AMainCharacter *MainCharacter = Cast<AMainCharacter>(OtherActor))
        {
            bOverlappingCombatSphere = false;
            if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
            {
                MoveToTarget(MainCharacter);  // ⚠️ Uses MainCharacter
                CombatTarget = nullptr;  // ❌ THEN CLEARS IT
            }
        }
    }
}
```

**Fixed Code:**
```cpp
void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
        if (MainCharacter)
        {
            bOverlappingCombatSphere = false;
            
            if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
            {
                MoveToTarget(MainCharacter);
            }
            
            // Clear combat target after using it
            CombatTarget = nullptr;  // ✅ CORRECT ORDER
        }
    }
}
```

**Validation:**
- [ ] Enemy chases player correctly when leaving combat range
- [ ] No null pointer access
- [ ] Enemy stops attacking when player leaves

---

### Step 2.2: Fix Critter Velocity Reset Timing 🏃
**File:** `GameplayActors/Critter.cpp`  
**Lines:** 35-42, 64-78  
**Time:** 15 minutes

**Current Code:**
```cpp
void ACritter::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
    const FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
    SetActorLocation(NewLocation);
    CurrentVelocity = FVector(0.0f);  // ❌ RESET TOO EARLY
}

void ACritter::Move(const FInputActionValue& Value)
{
    if (Controller != nullptr)
    {
        const FVector2d MoveValue = Value.Get<FVector2d>();
        // ... sets CurrentVelocity ...
    }
}
```

**Fixed Code:**
```cpp
void ACritter::Tick(const float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!CurrentVelocity.IsNearlyZero())
    {
        const FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
        SetActorLocation(NewLocation);
    }
}

void ACritter::Move(const FInputActionValue& Value)
{
    // Reset velocity at start of input
    CurrentVelocity = FVector(0.0f);  // ✅ RESET HERE INSTEAD
    
    if (Controller != nullptr)
    {
        const FVector2d MoveValue = Value.Get<FVector2d>();
        const FRotator MoveRotation(0, Controller->GetControlRotation().Yaw, 0);
        
        if (MoveValue.Y != 0.f)
        {
            const FVector Direction = MoveRotation.RotateVector(FVector::ForwardVector);
            CurrentVelocity.Y = FMath::Clamp(MoveValue.Y, -1.0f, 1.0f) * MaxSpeed;
        } 

        if(MoveValue.X != 0.f)
        {
            const FVector Direction = MoveRotation.RotateVector(FVector::RightVector);
            CurrentVelocity.X = FMath::Clamp(MoveValue.X, -1.0f, 1.0f) * MaxSpeed;
        }
    }
}
```

**Validation:**
- [ ] Critter movement is smooth and frame-rate independent
- [ ] No jittery movement
- [ ] Movement feels responsive

---

### Step 2.3: Fix Weapon State Update 🗡️
**File:** `GameplayActors/Weapon.cpp`  
**Line:** 44-75  
**Time:** 5 minutes

**Current Code:**
```cpp
void AWeapon::Equip(AMainCharacter* Char)
{
    if (Char)
    {
        // ... attachment code ...
        Char->SetEquippedWeapon(this);
        Char->SetActiveOverlappingItem(nullptr);
        
        // ❌ NEVER SETS WEAPON STATE
    }
}
```

**Fixed Code:**
```cpp
void AWeapon::Equip(AMainCharacter* Char)
{
    if (Char)
    {
        // Update weapon state first
        SetWeaponState(EWeaponState::Ews_Equipped);  // ✅ ADD THIS
        
        StaticMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
        StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
        StaticMesh->SetSimulatePhysics(false);

        if (const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket"))
        {
            RightHandSocket->AttachActor(this, Char->GetMesh());
            bRotate = false;
            
            if (AWeapon* CharWeapon = Char->GetEquippedWeapon())
            {
                CharWeapon->Destroy();	
            }
            
            Char->SetEquippedWeapon(this);
            Char->SetActiveOverlappingItem(nullptr);
        }

        if (OnEquipSound)
        {
            UGameplayStatics::PlaySound2D(this, OnEquipSound);
        }

        if (!bParticleWeapon)
        {
            IdleParticlesComponent->Deactivate();
        }
    }
}
```

**Validation:**
- [ ] Weapon state changes to Equipped
- [ ] Weapon behavior matches state
- [ ] Can query weapon state from Blueprint

---

### Step 2.4: Improve Sprint Logic 🏃‍♂️
**File:** `GameplayActors/MainCharacter.cpp`  
**Line:** 229-234  
**Time:** 10 minutes

**Current Code:**
```cpp
void AMainCharacter::StartSprinting(const FInputActionValue& Value)
{
    if (Value.Get<bool>() && GetVelocity().Size() > 0.0f && Stamina > 0.0f)  // ❌ REQUIRES MOVEMENT
    {
        bSprinting = true;
    }
}
```

**Fixed Code:**
```cpp
void AMainCharacter::StartSprinting(const FInputActionValue& Value)
{
    if (Value.Get<bool>())
    {
        // Can't sprint if exhausted
        if (StaminaStatus == EStaminaStatus::ESS_Exhausted || 
            StaminaStatus == EStaminaStatus::ESS_ExhaustedRecovering)
        {
            return;
        }
        
        // Can't sprint if dead
        if (bIsDead)
        {
            return;
        }
        
        // Can sprint even when standing still (will activate when moving)
        if (Stamina > 0.0f)
        {
            bSprinting = true;
        }
    }
}
```

**Also update StopSprinting:**
```cpp
void AMainCharacter::StopSprinting(const FInputActionValue& Value)
{
    if (!Value.Get<bool>() || bIsDead)  // ✅ Also check if dead
    {
        bSprinting = false;
    }
}
```

**Validation:**
- [ ] Can sprint from standstill
- [ ] Cannot sprint when exhausted
- [ ] Cannot sprint when dead
- [ ] Sprint behavior feels natural

---

## 🟠 PHASE 3: Medium Priority Improvements
**Time Estimate:** 1-2 hours  
**Risk:** Low - Code quality improvements

### Step 3.1: Replace Magic Numbers with Named Constants 🔢
**Files:** 
- `GameplayActors/MainCharacter.h`
- `GameplayActors/MainCharacter.cpp`
- `GameplayActors/Enemy.h`

**Time:** 30 minutes

**Add to MainCharacter.h (in public section):**
```cpp
// Camera Configuration
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
float CameraBoomDistance = 600.0f;

// Jump Configuration
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
float CharacterJumpVelocity = 650.0f;

// Animation Configuration
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
float Attack1PlayRate = 2.2f;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
float Attack2PlayRate = 1.8f;
```

**Update MainCharacter.cpp Constructor:**
```cpp
AMainCharacter::AMainCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = CameraBoomDistance;  // ✅ USE NAMED CONSTANT
    CameraBoom->bUsePawnControlRotation = true;

    // ... other setup ...

    GetCharacterMovement()->JumpZVelocity = CharacterJumpVelocity;  // ✅ USE NAMED CONSTANT
    
    // ... rest of constructor ...
}
```

**Update Attack() function:**
```cpp
void AMainCharacter::Attack()
{
    // ... null checks ...

    if (AnimInstance && CombatMontage)
    {
        switch (FMath::RandRange(0, 1))
        {
        case 0:
            AnimInstance->Montage_Play(CombatMontage, Attack1PlayRate);  // ✅ USE NAMED CONSTANT
            AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
            break;
        case 1:
            AnimInstance->Montage_Play(CombatMontage, Attack2PlayRate);  // ✅ USE NAMED CONSTANT
            AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
            break;
        default:
            break;
        }
    }
}
```

**Add to Enemy.h:**
```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
float AgroSphereRadius = 600.0f;

UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
float CombatSphereRadius = 75.0f;
```

**Update Enemy.cpp Constructor:**
```cpp
AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
    AgroSphere->SetupAttachment(GetRootComponent());
    AgroSphere->SetSphereRadius(AgroSphereRadius);  // ✅ USE NAMED CONSTANT

    CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
    CombatSphere->SetupAttachment(GetRootComponent());
    CombatSphere->SetSphereRadius(CombatSphereRadius);  // ✅ USE NAMED CONSTANT

    bOverlappingCombatSphere = false;
}
```

**Validation:**
- [ ] All magic numbers replaced with properties
- [ ] Properties are editable in Blueprint editor
- [ ] Values behave the same as before
- [ ] Can tweak values easily for game balancing

---

### Step 3.2: Clean Up Debug Logging 🧹
**Files:** 
- `GameplayActors/Item.cpp`
- `GameplayActors/Pickup.cpp`
- `GameplayActors/Explosive.cpp`
- `GameplayActors/FloorSwitch.cpp`

**Time:** 15 minutes

**Wrap all debug logs with shipping check:**

**Item.cpp:**
```cpp
void AItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                           bool bFromSweep, const FHitResult& SweepResult)
{
#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Verbose, TEXT("AItem::OnOverlapBegin() - %s"), *GetName());
#endif

    if (OverlapParticles)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), OverlapParticles, 
            GetActorLocation(), FRotator(0.f), FVector(1.f), true, true, ENCPoolMethod::AutoRelease);
    }

    if (OverlapSound)
    {
        UGameplayStatics::PlaySound2D(this, OverlapSound);
    }
}

void AItem::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
#if !UE_BUILD_SHIPPING
    UE_LOG(LogTemp, Verbose, TEXT("AItem::OnOverlapEnd() - %s"), *GetName());
#endif
}
```

**Apply same pattern to:**
- Pickup.cpp (lines 16, 29)
- Explosive.cpp (lines 16, 27)
- FloorSwitch.cpp (lines 62, 70)

**Validation:**
- [ ] Logs still appear in development builds
- [ ] Logs are removed in shipping builds
- [ ] No performance impact in shipping

---

### Step 3.3: Fix Const Correctness 📐
**Files:** 
- `GameplayActors/Enemy.h`
- `GameplayActors/Enemy.cpp`

**Time:** 5 minutes

**Enemy.h - Line 72:**
```cpp
UFUNCTION(BlueprintCallable)
void MoveToTarget(const AMainCharacter* Target) const;  // ✅ ADD CONST
```

**Enemy.cpp - Line 118:**
```cpp
void AEnemy::MoveToTarget(const AMainCharacter* Target) const  // ✅ ADD CONST
{
    // Implementation stays the same
    // Function doesn't modify Enemy state, only issues move command
}
```

**Validation:**
- [ ] Code compiles
- [ ] AI movement still works
- [ ] Function is properly const

---

## 🟢 PHASE 4: Low Priority / Polish
**Time Estimate:** 30-60 minutes  
**Risk:** Very Low - Nice to have

### Step 4.1: Make Collider Mesh Configurable 🎨
**Files:** 
- `GameplayActors/Collider.h`
- `GameplayActors/Collider.cpp`

**Time:** 10 minutes

**Add to Collider.h:**
```cpp
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
UStaticMesh* ColliderMesh;
```

**Update Collider.cpp Constructor:**
```cpp
ACollider::ACollider()
{
    PrimaryActorTick.bCanEverTick = true;

    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    SetRootComponent(SphereComponent);
    SphereComponent->InitSphereRadius(40.0f);
    SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(GetRootComponent());
    
    // Set default mesh if available
    static ConstructorHelpers::FObjectFinder<UStaticMesh>
    DefaultMesh(TEXT("/Script/Engine.StaticMesh'/Game/InfinityBladeAdversaries/Maps/LevelContent/Architecture/SM_CycRoom_01.SM_CycRoom_01'"));
    
    if (DefaultMesh.Succeeded())
    {
        ColliderMesh = DefaultMesh.Object;
    }
}

void ACollider::BeginPlay()
{
    Super::BeginPlay();
    
    // Apply mesh at BeginPlay
    if (ColliderMesh)
    {
        MeshComponent->SetStaticMesh(ColliderMesh);
        MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
        MeshComponent->SetWorldScale3D(FVector(0.2f));
    }
}
```

**Validation:**
- [ ] Mesh is editable in editor
- [ ] Can assign different meshes
- [ ] Default mesh still loads if nothing assigned

---

### Step 4.2: Remove Commented Code 🗑️
**File:** `GameplayActors/Floater.cpp`  
**Lines:** 51-53, 58-83  
**Time:** 5 minutes

**Remove these commented sections:**
```cpp
// DELETE THESE LINES:
/*StaticMesh->AddForce(InitialForce);
StaticMesh->AddTorqueInDegrees(InitialTorqueInDegrees);
StaticMesh->AddTorqueInRadians(InitialTorqueInRadians);*/

// DELETE THESE LINES:
// const FRotator Rotation = FRotator(0.0f, 0.0f, 1.0f);
// ActorWorldRotation
//AddActorWorldRotation(Rotation);
// ... etc ...
```

**Validation:**
- [ ] Code still compiles
- [ ] Floater still works correctly
- [ ] File is cleaner and more readable

---

### Step 4.3: Optimize Tick Usage ⚡
**Files:** 
- `GameplayActors/Floater.cpp`
- `GameplayActors/FloatingPlatform.cpp`

**Time:** 15 minutes

**Floater.cpp:**
```cpp
void AFloater::BeginPlay()
{
    Super::BeginPlay();
    
    // ... existing code ...

    // Only enable tick if needed
    SetActorTickEnabled(bShouldFloat);  // ✅ ADD THIS
}

// Add public helper function in Floater.h:
UFUNCTION(BlueprintCallable, Category = "Floater")
void SetFloating(bool bNewShouldFloat)
{
    bShouldFloat = bNewShouldFloat;
    SetActorTickEnabled(bShouldFloat);
}
```

**FloatingPlatform.cpp:**
```cpp
void AFloatingPlatform::BeginPlay()
{
    Super::BeginPlay();

    StartPoint = GetActorLocation();
    EndPoint += StartPoint;
    
    // Start with tick disabled
    SetActorTickEnabled(false);  // ✅ ADD THIS
    
    GetWorldTimerManager().SetTimer(InterpTimer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
    Distance = (EndPoint - StartPoint).Size();
}

void AFloatingPlatform::ToggleInterping()
{
    bInterping = !bInterping;
    SetActorTickEnabled(bInterping);  // ✅ ADD THIS
}
```

**Validation:**
- [ ] Floater only ticks when floating
- [ ] Platform only ticks when moving
- [ ] Performance improves with many actors
- [ ] Behavior is unchanged

---

### Step 4.4: Update Copyright Comments 📝
**All Files**  
**Time:** 10 minutes

**Replace this:**
```cpp
// Fill out your copyright notice in the Description page of Project Settings.
```

**With this:**
```cpp
// FirstCplusProject - Unreal Engine C++ Learning Project
// Created: 2025
```

**Or customize to your preference!**

**Validation:**
- [ ] All files updated
- [ ] Consistent header style

---

## ✅ Testing Checklist

After completing all fixes, test these scenarios:

### Critical Functionality
- [ ] Game launches without crashes
- [ ] Player can move, jump, sprint correctly
- [ ] Player can equip weapons
- [ ] Player can attack with weapon
- [ ] Player death works properly
- [ ] HUD displays (or fails gracefully if not set)

### Combat & AI
- [ ] Enemy detects player
- [ ] Enemy chases player
- [ ] Enemy enters combat mode
- [ ] Enemy stops when player dies/leaves

### Items & Pickups
- [ ] Can pickup coins
- [ ] Can pickup weapons
- [ ] Explosives damage player
- [ ] Item particles/sounds work

### Performance
- [ ] No performance issues with multiple actors
- [ ] Tick optimization works
- [ ] No memory leaks

### Edge Cases
- [ ] Sprint when exhausted (should fail)
- [ ] Attack without weapon (should fail gracefully)
- [ ] Die multiple times (should be safe)
- [ ] Equip weapon while attacking
- [ ] Null pointer scenarios don't crash

---

## 📊 Progress Tracking

### Phase 1: Critical Fixes
- [ ] Step 1.1: MainPlayerController null check
- [ ] Step 1.2: Floater initialization
- [ ] Step 1.3: Die() function
- [ ] Step 1.4: MainCharacter null checks

### Phase 2: High Priority
- [ ] Step 2.1: Enemy combat logic
- [ ] Step 2.2: Critter velocity
- [ ] Step 2.3: Weapon state
- [ ] Step 2.4: Sprint logic

### Phase 3: Medium Priority
- [ ] Step 3.1: Magic numbers
- [ ] Step 3.2: Debug logging
- [ ] Step 3.3: Const correctness

### Phase 4: Low Priority
- [ ] Step 4.1: Collider mesh
- [ ] Step 4.2: Commented code
- [ ] Step 4.3: Tick optimization
- [ ] Step 4.4: Copyright comments

---

## 🚨 Rollback Plan

If any fix causes issues:

1. **Git:** Commit after each phase
   ```bash
   git commit -m "Phase 1: Critical fixes complete"
   ```

2. **Backup:** Keep backup of original files
3. **Test:** Test after each step before moving on
4. **Revert:** If needed, revert specific commits

---

## 📚 Additional Resources

### Unreal Engine Best Practices
- [Unreal C++ Coding Standard](https://docs.unrealengine.com/5.0/en-US/epic-cplusplus-coding-standard-for-unreal-engine/)
- [Gameplay Framework](https://docs.unrealengine.com/5.0/en-US/gameplay-framework-in-unreal-engine/)
- [Performance Guidelines](https://docs.unrealengine.com/5.0/en-US/performance-guidelines-for-unreal-engine/)

### Next Steps After Fixes
1. Implement proper combat system
2. Add enemy health/damage
3. Create save/load system
4. Add UI polish
5. Implement multiplayer (if needed)

---

## 📝 Notes

- **Backup:** Create a backup before starting!
- **Compile:** Compile after each fix
- **Test:** Test functionality after each fix
- **Commit:** Commit after each phase
- **Ask:** If stuck, ask for help!

**Good luck with the fixes! 🚀**

