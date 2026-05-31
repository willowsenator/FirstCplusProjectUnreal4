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
	
	static auto MakeCombatWorld() -> FCombatWorldFixture;
};