// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floater.generated.h"

UCLASS()
class FIRSTCPLUSPROJECT_API AFloater : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloater();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="ActorMeshComponents")
	UStaticMeshComponent *StaticMesh;

	// Initial force applied to the mesh
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Floater Variables")
	FVector InitialForce;

	// Initial torque in degrees applied to the mesh
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Floater Variables")
	FVector InitialTorqueInDegrees;
	
	// Initial torque in radians applied to the mesh
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Floater Variables")
	FVector InitialTorqueInRadians;
	
	// Location used by SetActorLocation() when BeginPlay() is called
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Floater Variables")
	FVector InitialLocation;

	// Location of the actor when dragged in from the editor
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Floater Variables")
	FVector PlacedLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Floater Variables")
	FVector WorldOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floater Variables")
	FVector InitialDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Floater Variables")
	bool bShouldFloat;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Floater Variables")
	bool bInitializeFloaterLocations;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
