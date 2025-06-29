// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "MainCharacter.h"

APickup::APickup()
{
	CoinCount = 1;
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	UE_LOG(LogTemp, Warning, TEXT("APickup::OnOverlapBegin()"));

	if (OtherActor)
	{
		if (auto const MainCharacter = Cast<AMainCharacter>(OtherActor))
		{
			MainCharacter->IncrementCoins(CoinCount);
			MainCharacter->PickupLocations.Add(GetActorLocation());
			Destroy();
		}
	}
}


void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	UE_LOG(LogTemp, Warning, TEXT("APickup::OnOverlapEnd()"));
}
