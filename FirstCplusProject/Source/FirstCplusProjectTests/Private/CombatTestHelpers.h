#pragma once
#include "CoreMinimal.h"

class UWorld;
class AEnemy;
class AMainCharacter;
class AMainPlayerController;


struct FCombatWorldFixture
{
	UWorld* World = nullptr;
	AEnemy* Enemy = nullptr;
	AMainCharacter* MainCharacter = nullptr;
	AMainPlayerController* PlayerController = nullptr;
	
	/** Tears down */
	void Destroy();
	
	/**
	 * Builds the world. A game mode (and the game instance it needs) is only created on request:
	 * APawn::TakeDamage refuses every hit in a world with no authority game mode.
	 */
	static auto MakeCombatWorld(bool bWithGameMode = false) -> FCombatWorldFixture;
};