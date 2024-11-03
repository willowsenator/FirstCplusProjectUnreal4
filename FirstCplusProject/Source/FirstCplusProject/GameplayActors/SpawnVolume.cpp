// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnVolume::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	const auto SpawnOrigin = SpawnBox->Bounds.Origin;
	const auto SpawnExtent = SpawnBox->Bounds.BoxExtent;

	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
}

void ASpawnVolume::SpawnOurPawn_Implementation(UClass *ToSpawn, const FVector &Location)
{
	if(ToSpawn)
	{
		if (const auto World = GetWorld())
		{
			const auto SpawnParams = FActorSpawnParameters();
			World->SpawnActor<ACritter>(ToSpawn, Location, FRotator(0.0f), SpawnParams);
		}
	}
}

