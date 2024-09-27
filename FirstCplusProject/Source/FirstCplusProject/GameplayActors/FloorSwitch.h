// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "FloorSwitch.generated.h"

UCLASS()
class FIRSTCPLUSPROJECT_API AFloorSwitch : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AFloorSwitch();

	/** Overlap volume for functionality to be triggered */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UBoxComponent* TriggerBox;

	/** Switch for the character to step on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UStaticMeshComponent* FloorSwitch;

	/** Tree to move when the floor switch is stepped on */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floor Switch")
	UStaticMeshComponent* Tree;

	/** Initial location for the floor switch */
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialSwitchLocation;

	/** Initial location for the tree */
	UPROPERTY(BlueprintReadWrite, Category = "Floor Switch")
	FVector InitialTreeLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void MoveUpTree();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void MoveDownTree();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void MoveUpFloorSwitch();

	UFUNCTION(BlueprintImplementableEvent, Category = "Floor Switch")
	void MoveDownFloorSwitch();

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateTreeLocation(float Z) const;

	UFUNCTION(BlueprintCallable, Category = "Floor Switch")
	void UpdateFloorSwitchLocation(float Z) const;
};
