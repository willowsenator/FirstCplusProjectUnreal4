// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	StaticMesh->SetupAttachment(GetRootComponent());
	bParticleWeapon = false;
}


void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, const int32 OtherBodyIndex, const bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor)
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
		StaticMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		StaticMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

		StaticMesh->SetSimulatePhysics(false);

		if (const USkeletalMeshSocket* RightHandSocket = Char->GetMesh()->GetSocketByName("RightHandSocket"))
		{
			RightHandSocket->AttachActor(this, Char->GetMesh());
			bRotate = false;
			Char->SetEquippedWeapon(this);
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
