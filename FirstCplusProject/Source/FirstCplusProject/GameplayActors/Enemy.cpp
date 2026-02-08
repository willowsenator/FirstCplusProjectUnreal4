// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);
	
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("weapon_socket"));

	bOverlappingCombatSphere = false;
	CombatTarget = nullptr;
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	Health = 75.f;
	MaxHealth = 100.f;
	Damage = 10.f;
	
	bCanAttack = true;
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
	
	// Configure the box collision for weapon overlaps
	CombatCollision->SetCollisionObjectType(ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CombatCollision->SetGenerateOverlapEvents(true);
	// Start with collision disabled until attack is performed
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::SetEnemyMovementStatus(const EEnemyMovementStatus NewStatus)
{
	EnemyMovementStatus = NewStatus;
}

// Called every frame
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
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("MainCharacter detected in combat sphere - calling Attack()"));
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

			// Only clear combat target if the leaving pawn was our target and no longer in agro range.
			if (CombatTarget == MainCharacter)
			{
				if (!AgroSphere || !AgroSphere->IsOverlappingActor(MainCharacter))
				{
					CombatTarget = nullptr;
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

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(5.f);

	FNavPathSharedPtr NavPath;

	AIController->MoveTo(MoveRequest, &NavPath);

	/** For DEBUG Purposes
	 * for (auto PathPoints = NavPath->GetPathPoints(); const auto PathPoint : PathPoints)
	{
		const FVector Location = PathPoint.Location;

		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 1.5f);
	}*/
}

void AEnemy::AttackEnd()
{
	bCanAttack = true;

	if (bOverlappingCombatSphere && CombatTarget)
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemy::Attack, FMath::RandRange(0.5f, 1.2f), false);
		return;
	}

	if (AgroSphere && CombatTarget && AgroSphere->IsOverlappingActor(CombatTarget) && EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		MoveToTarget(CombatTarget);
	}
	else
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		if (AIController)
		{
			AIController->StopMovement();
		}
	}
}

void AEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != CombatMontage)
	{
		return;
	}

	AttackEnd();
}

void AEnemy::Attack()
{
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
		if (UAnimInstance *AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && CombatMontage)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);

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

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	if (OtherActor == GetOwner()) return; // Ignore if overlapping with the weapon's owner
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
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::CombatOnOverlapEnd()") );
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
