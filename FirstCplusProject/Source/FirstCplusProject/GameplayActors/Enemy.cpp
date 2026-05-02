// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Movement resumption is event-driven (AttackEnd / *OnOverlapEnd); no per-frame work needed.
	PrimaryActorTick.bCanEverTick = false;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);
	
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), TEXT("weapon_socket"));
	CombatCollision->SetCollisionObjectType(ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CombatCollision->SetGenerateOverlapEvents(true);
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bOverlappingCombatSphere = false;
	CombatTarget = nullptr;
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	Health = 75.f;
	MaxHealth = 100.f;
	Damage = 10.f;
	
	bCanAttack = true;
	
	// Steer around other AIs instead of pushing through their capsules
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceWeight = 0.5f;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 500.f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);
	
	// Bind combat collision overlap events
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);
}

void AEnemy::SetEnemyMovementStatus(const EEnemyMovementStatus NewStatus)
{
	EnemyMovementStatus = NewStatus;
}

// Tick is disabled (see PrimaryActorTick.bCanEverTick = false in the constructor).
void AEnemy::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor))
		{
			CombatTarget = MainCharacter;
			MoveToTarget(MainCharacter);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (const AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor))
		{
			if (CombatTarget == MainCharacter)
			{
				CombatTarget = nullptr;
			}

			if (AIController)
			{
				AIController->StopMovement();
			}

			// Clear AttackTimer
			GetWorldTimerManager().ClearTimer(AttackTimerHandle);
			// Important: Set status to Idle so animation blends back properly
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor))
		{
			bOverlappingCombatSphere = true;
			CombatTarget = MainCharacter;
			Attack();
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		if (const AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor))
		{
			bOverlappingCombatSphere = false;
			GetWorldTimerManager().ClearTimer(AttackTimerHandle);

			// Only clear combat target if the leaving pawn was our target and no longer in agro range.
			if (CombatTarget == MainCharacter)
			{
				if (!AgroSphere || !AgroSphere->IsOverlappingActor(MainCharacter))
				{
					CombatTarget = nullptr;
					SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
				}
				else
				{
					// Still in agro range, return to moving - clear EMS_Attacking first
					SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
					MoveToTarget(MainCharacter);
				}
			}
		}
	}
}

void AEnemy::MoveToTarget(const AMainCharacter* Target)
{
	if (!AIController || !Target)
	{
		return;
	}

	if (const UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr; EnemyMovementStatus == EEnemyMovementStatus::EMS_Attacking || (AnimInstance && CombatMontage && AnimInstance->Montage_IsPlaying(CombatMontage)))
	{
		return;
	}

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(60.f); // stop just inside the 75-radius combat sphere

	FNavPathSharedPtr NavPath;

	AIController->MoveTo(MoveRequest, &NavPath);

	/** For DEBUG Purposes
	 * for (auto PathPoints = NavPath->GetPathPoints(); const auto PathPoint : PathPoints)
	{
		const FVector Location = PathPoint.Location;

		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 1.5f);
	}*/
}

void AEnemy::Attack()
{
	if (!bOverlappingCombatSphere || !CombatTarget) return;
	
	if (AIController)
	{
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	}

	// Turn to face the combat target
	if (CombatTarget)
	{
		const FVector DirectionToTarget = (CombatTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		const FRotator RotationToTarget = DirectionToTarget.Rotation();
		SetActorRotation(FRotator(0.0f, RotationToTarget.Yaw, 0.0f));
	}

	if (bCanAttack)
	{
		bCanAttack = false;
		bAttackEndHandled = false;
		if (UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);

			// Bind the montage end callback so AttackEnd() gets called when montage finishes
			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AEnemy::OnAttackMontageEnded);
			AnimInstance->Montage_SetEndDelegate(EndDelegate, CombatMontage);
			
			if (SwingSound)
			{
				UGameplayStatics::PlaySound2D(this, SwingSound);
			}
		}
	}
	
}

void AEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != CombatMontage || bInterrupted || !IsValid(this) || IsActorBeingDestroyed()) return;
	AttackEnd();
}

void AEnemy::AttackEnd()
{
	if (bAttackEndHandled)
	{
		// Already processed by montage end or notify.
		return;
	}

	bAttackEndHandled = true;
	bCanAttack = true;

	// If still in combat sphere, schedule next attack
	if (bOverlappingCombatSphere && CombatTarget)
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemy::Attack, FMath::RandRange(0.5f, 1.2f), false);
		return;
	}

	// If still in agro sphere, return to moving toward target
	if (AgroSphere && CombatTarget && AgroSphere->IsOverlappingActor(CombatTarget))
	{
		// Clear EMS_Attacking so MoveToTarget's status gate won't early-return
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		MoveToTarget(CombatTarget);
	}
	else
	{
		// Out of agro range, stop and go idle
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		if (AIController)
		{
			AIController->StopMovement();
		}
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	if (!OtherActor || OtherActor == this) return;
	if (Cast<AEnemy>(OtherActor)) return; // skip enemy vs enemy
	if (const AMainCharacter *MainCharacter = Cast<AMainCharacter>(OtherActor); MainCharacter)
	{
		if (MainCharacter->HitParticles)
		{
			// Prefer socket location on the static mesh if it exists
			if (const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket"))
			{
				const FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainCharacter->HitParticles, SocketLocation, FRotator::ZeroRotator, FVector(1.f), false);
			}
		}
		// Optionally apply damage here
		
		if (MainCharacter->HitSound)
		{
			UGameplayStatics::PlaySound2D(this, MainCharacter->HitSound);
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AEnemy::ActivateCollision() const
{
	if (CombatCollision)
	{
		CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AEnemy::DeactivateCollision() const
{
	if (CombatCollision)
	{
		CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
