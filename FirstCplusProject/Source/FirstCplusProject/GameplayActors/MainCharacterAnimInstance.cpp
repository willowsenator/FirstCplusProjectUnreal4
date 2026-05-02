// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterAnimInstance.h"

#include "GameFramework/PawnMovementComponent.h"
#include "KismetAnimationLibrary.h"

void UMainCharacterAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
	}
}

void UMainCharacterAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		const FVector Velocity = Pawn->GetVelocity();
		const FVector LateralVelocity = FVector(Velocity.X, Velocity.Y, 0.0f);
		MovementSpeed = LateralVelocity.Size();

		Direction = UKismetAnimationLibrary::CalculateDirection(LateralVelocity, Pawn->GetActorRotation());

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
		
		bIsSprinting = MainCharacter && MainCharacter->MovementStatus == EMovementStatus::EMS_Sprinting && MovementSpeed > 10.f;

		if (MainCharacter == nullptr )
		{
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
	}
}
