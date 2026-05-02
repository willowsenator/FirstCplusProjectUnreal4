// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Enemy.h"
#include "MainCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h" // for UStaticMeshComponent declarations
#include "Engine/SkeletalMeshSocket.h" // for USkeletalMeshSocket
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	StaticMesh->SetupAttachment(GetRootComponent());
	
	CombatCollision = CreateDefaultSubobject<UBoxComponent>("CombatCollision");
	// Attach the collision to the StaticMesh so it follows the mesh transforms when the actor is attached to a socket
	CombatCollision->SetupAttachment(StaticMesh);
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
	
	// Ensure the box collision is active for overlaps
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECC_WorldDynamic);
	// Start with ignoring everything then enable overlap with Pawns
	CombatCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// Allow overlap events
	CombatCollision->SetGenerateOverlapEvents(true);
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
	if (OtherActor == GetOwner()) return; // Ignore if overlapping with the weapon's owner
	if (const AEnemy *Enemy = Cast<AEnemy>(OtherActor); Enemy)
	{
		if (Enemy->HitParticles)
		{
			// Prefer socket location on the static mesh if it exists
			if (StaticMesh)
			{
				const FVector SocketLocation = StaticMesh->GetSocketLocation(TEXT("WeaponSocket"));
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->HitParticles, SocketLocation, FRotator::ZeroRotator, FVector(1.f), false);
			}
		}
		// Optionally apply damage here
		
		if (Enemy->HitSound)
		{
			UGameplayStatics::PlaySound2D(this, Enemy->HitSound);
		}
	}
}

void AWeapon::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AWeapon::ActivateCollision() const
{
	if (CombatCollision)
	{
		CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AWeapon::DeactivateCollision() const
{
	if (CombatCollision)
	{
		CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
