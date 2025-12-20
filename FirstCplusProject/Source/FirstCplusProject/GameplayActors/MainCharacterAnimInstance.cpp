// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterAnimInstance.h"

#include "GameFramework/PawnMovementComponent.h"

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

		// Calculate Direction (degrees) relative to actor forward without using UKismet helpers
		if (LateralVelocity.SizeSquared() > 0.f)
		{
			// Transform world velocity into the actor's local space
			const FRotator ActorRotation = Pawn->GetActorRotation();
			const FVector LocalVelocity = ActorRotation.UnrotateVector(LateralVelocity);
			Direction = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
		}
		else
		{
			Direction = 0.f;
		}

		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (MainCharacter == nullptr )
		{
			MainCharacter = Cast<AMainCharacter>(Pawn);
		}
	}
}
