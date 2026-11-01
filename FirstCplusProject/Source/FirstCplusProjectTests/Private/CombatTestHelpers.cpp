#include "CombatTestHelpers.h"

#include "FirstCplusProject/GameplayActors/Enemy.h"
#include "FirstCplusProject/GameplayActors/MainCharacter.h"
#include "FirstCplusProject/GameplayActors/MainPlayerController.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

FCombatWorldFixture FCombatWorldFixture::MakeCombatWorld(const bool bWithGameMode)
{
	FCombatWorldFixture Fixture;

	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);

	if (!World)
	{
		return Fixture;
	}


	FWorldContext& Context = GEngine->CreateNewWorldContext(EWorldType::Game);
	Context.SetCurrentWorld(World);

	if (bWithGameMode)
	{
		UGameInstance* GameInstance = NewObject<UGameInstance>(GEngine);
		Context.OwningGameInstance = GameInstance;
		World->SetGameInstance(GameInstance);
		GameInstance->Init();
		World->SetGameMode(FURL());
	}

	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Fixture.World = World;
	Fixture.MainCharacter = World->SpawnActor<AMainCharacter>(
		FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	Fixture.Enemy = World->SpawnActor<AEnemy>(FVector(500.f, 0.f, 0.f), FRotator::ZeroRotator, SpawnParams);
	Fixture.PlayerController = World->SpawnActor<AMainPlayerController>(
		FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	
	if (Fixture.PlayerController && Fixture.MainCharacter)
	{
		Fixture.PlayerController->Possess(Fixture.MainCharacter);
	}
		
	return Fixture;
}

void FCombatWorldFixture::Destroy()
{
	if (!World)
	{
		return;
	}
	
	if (UGameInstance* GameInstance = World->GetGameInstance())
	{
		GameInstance->Shutdown();
	}

	GEngine->DestroyWorldContext(World);
	World->DestroyWorld(false);
	
	World = nullptr;
	Enemy = nullptr;
	MainCharacter = nullptr;
	PlayerController = nullptr;	
}
