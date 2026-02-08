// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
		Enemy = Cast<AEnemy>(Pawn);	
	}
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	UpdateAnimationProperties();
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	// Always try to get pawn owner if we don't have it
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		// Calculate velocity and movement speed
		const FVector Velocity = Pawn->GetVelocity();
		const FVector LateralVelocity = FVector(Velocity.X, Velocity.Y, 0.f);
		MovementSpeed = LateralVelocity.Size();

		// Get or cast to Enemy reference
		if (Enemy == nullptr)
		{
			Enemy = Cast<AEnemy>(Pawn);
		}

		// Always sync the movement status from the enemy
		if (Enemy)
		{
			EnemyMovementStatus = Enemy->EnemyMovementStatus;
		}
	}
	else
	{
		// If pawn is invalid, reset values
		MovementSpeed = 0.0f;
		EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;
	}
}

