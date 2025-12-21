// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Enemy.h"
#include "MainCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AWeapon::AWeapon()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	StaticMesh->SetupAttachment(GetRootComponent());
	
	CombatCollision = CreateDefaultSubobject<UBoxComponent>("CombatCollision");
	// Attach the collision to the StaticMesh so it follows the mesh transforms when the actor is attached to a socket
	CombatCollision->SetupAttachment(GetRootComponent());
	// Ensure the box collision is active for overlaps
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CombatCollision->SetCollisionObjectType(ECC_WorldDynamic);
	// Start with ignoring everything then enable overlap with Pawns
	CombatCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CombatCollision->SetGenerateOverlapEvents(true);
	// Default extent in case user didn't set it in the editor (helps debugging)
	CombatCollision->SetBoxExtent(FVector(32.f, 8.f, 8.f));
	
	bParticleWeapon = false;
	
	WeaponState = EWeaponState::Ews_Pickup;
	
	Damage = 25.f;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AWeapon::CombatOnOverlapEnd);
	// Debug: print collision state and extent
	if (CombatCollision)
	{
		const auto Enabled = CombatCollision->GetCollisionEnabled() == ECollisionEnabled::QueryOnly ? TEXT("QueryOnly") : (CombatCollision->GetCollisionEnabled() == ECollisionEnabled::NoCollision ? TEXT("NoCollision") : TEXT("PhysicsAndQuery"));
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::BeginPlay - CombatCollision Enabled=%s Extent=%s Loc=%s Rot=%s"), Enabled, *CombatCollision->GetScaledBoxExtent().ToString(), *CombatCollision->GetComponentLocation().ToString(), *CombatCollision->GetComponentRotation().ToString());
	}
}


void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (WeaponState == EWeaponState::Ews_Pickup && OtherActor)
	{
		if (AMainCharacter* MyCharacter = Cast<AMainCharacter>(OtherActor))
		{
			// If the character is valid, set it as the active overlapping item
			MyCharacter->ActiveOverlappingItem = this;
		}
	}
	// Additional logic for weapon overlap can be added here
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	// Additional logic for weapon overlap end can be added here
	if (AMainCharacter * MyCharacter = Cast<AMainCharacter>(OtherActor))
	{
		// If the character is valid, clear the active overlapping item
		MyCharacter->ActiveOverlappingItem = nullptr;
	}
}

void AWeapon::Equip(AMainCharacter* Char)
{
	if (Char)
	{
		// Update weapon state first
		SetWeaponState(EWeaponState::Ews_Equipped);
		
		StaticMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

		StaticMesh->SetSimulatePhysics(false);

		if (const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket"))
		{
			RightHandSocket->AttachActor(this, Char->GetMesh());
			// Prevent the actor from rotating while attached
			bRotate = false;
			// Set the weapon owner to the character to avoid self-hits
			SetOwner(Char);
			if (AWeapon *CharWeapon = Char->GetEquippedWeapon())
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

void AWeapon::CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	
	// Ignore hits on the weapon owner (e.g., the player)
	if (OtherActor == GetOwner())
	{
		return;
	}

	if (const AEnemy *Enemy = Cast<AEnemy>(OtherActor); Enemy)
	{
		if (Enemy->HitParticles)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Enemy->HitParticles, GetActorLocation(), FRotator(0.f), FVector(1.f), false, true, ENCPoolMethod::ManualRelease);
		}
		// Optionally apply damage here
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::CombatOnOverlapEnd()") );
}

void AWeapon::ActivateCollision() const
{
	if (CombatCollision)
	{
		CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		CombatCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
		CombatCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		CombatCollision->SetGenerateOverlapEvents(true);
	}
}

void AWeapon::DeactivateCollision() const
{
	if (CombatCollision)
	{
		CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CombatCollision->SetGenerateOverlapEvents(false);
	}
}
