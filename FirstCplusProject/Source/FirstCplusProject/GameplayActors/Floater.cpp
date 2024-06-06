// Fill out your copyright notice in the Description page of Project Settings.


#include "Floater.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomStaticMesh"));
	InitialLocation = FVector(0.0f);
	PlacedLocation = FVector(0.0f);
	WorldOrigin = FVector(0.0f, 0.0f, 0.0f);
	InitialDirection = FVector(0.0f, 0.0f, 0.0f);
	
	bInitializeFloaterLocations = false;
	bShouldFloat = false;
}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();

	/*const float InitialX = FMath::Rand();
	const float InitialY = FMath::Rand();
	const float InitialZ = FMath::Rand();*/
	const float InitialX = FMath::FRandRange(-500.f, 500.f);
	const float InitialY = FMath::FRandRange(-500.f, 500.f);
	const float InitialZ = FMath::FRandRange(0.f, 500.f);
	

	InitialLocation.X = InitialX;
	InitialLocation.Y = InitialY;
	InitialLocation.Z = InitialZ;

	//InitialLocation *= 500.f;
	
	PlacedLocation = GetActorLocation();

	if(bInitializeFloaterLocations)
	{
		SetActorLocation(InitialLocation);
	}

	/*StaticMesh->AddForce(InitialForce);
	StaticMesh->AddTorqueInDegrees(InitialTorqueInDegrees);
	StaticMesh->AddTorqueInRadians(InitialTorqueInRadians);*/
}

// Called every frame
void AFloater::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bShouldFloat)
	{
		FHitResult HitResult;
		AddActorLocalOffset(InitialDirection, true, &HitResult);
		const FVector HitLocation = HitResult.Location;
		UE_LOG(LogTemp, Warning, TEXT("Hit Location: X = %f, Y = %f, Z = %f"),
			HitLocation.X, HitLocation.Y, HitLocation.Z);
	}

	// const FRotator Rotation = FRotator(0.0f, 0.0f, 1.0f);
	// ActorWorldRotation
	//AddActorWorldRotation(Rotation);

	// ActorLocalRotation
	//AddActorLocalRotation(Rotation);

	// ActorWorldOffset
	// const FVector WorldOffset = FVector(200.0f, 0.0f, 0.0f);
	// FHitResult HitResult;
	// AddActorWorldOffset(WorldOffset, true, &HitResult);

	// ActorLocalOffset
	// const FVector LocalOffset = FVector(0.0f, 0.0f, 200.0f);
	// FHitResult HitResult2;
	// AddActorLocalOffset(LocalOffset, true, &HitResult2);
}

