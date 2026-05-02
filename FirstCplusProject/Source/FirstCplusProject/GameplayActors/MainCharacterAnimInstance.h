// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "Animation/AnimInstance.h"
#include "MainCharacterAnimInstance.generated.h"

class UBlendSpace;

/**
 * 
 */
UCLASS()
class FIRSTCPLUSPROJECT_API UMainCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable, Category=AnimationProperties)
	void UpdateAnimationProperties();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Direction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsInAir;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	AMainCharacter* MainCharacter;

	// Expose a BlendSpace asset so you can set it in the AnimBlueprint class defaults
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UBlendSpace* LocomotionBlendSpace;
};
