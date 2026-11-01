#include "CombatTestHelpers.h"
#include "FirstCplusProject/GameplayActors/Enemy.h"
#include "FirstCplusProject/GameplayActors/MainCharacter.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_CombatTargetClearedWhenLeavingAgro, "FirstCplusProject.Tests.MainCharacter.CombatTarget.ClearedWhenLeavingAgro",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_CombatTargetClearedWhenLeavingAgro::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy) || !TestNotNull(TEXT("MainCharacter spawned"), Fixture.MainCharacter))
	{
		Fixture.Destroy();
		return false;
	}

	Fixture.MainCharacter->SetCombatTarget(Fixture.Enemy);
	Fixture.MainCharacter->SetHasCombatTarget(true);

	Fixture.Enemy->AgroSphereOnOverlapEnd(nullptr, Fixture.MainCharacter, nullptr, 0);

	TestNull(TEXT("Leaving the agro sphere releases the player's combat target"), Fixture.MainCharacter->CombatTarget);
	TestFalse(TEXT("Leaving the agro sphere clears the combat flag"), Fixture.MainCharacter->bHasCombatTarget);

	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_CombatTargetNoTargetStaysClear, "FirstCplusProject.Tests.MainCharacter.CombatTarget.NoTargetStaysClear",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_CombatTargetNoTargetStaysClear::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy) || !TestNotNull(TEXT("MainCharacter spawned"), Fixture.MainCharacter))
	{
		Fixture.Destroy();
		return false;
	}

	// The player never had a target: leaving an enemy's agro sphere must leave the state clear.
	Fixture.MainCharacter->SetCombatTarget(nullptr);
	Fixture.MainCharacter->SetHasCombatTarget(false);

	Fixture.Enemy->AgroSphereOnOverlapEnd(nullptr, Fixture.MainCharacter, nullptr, 0);

	TestNull(TEXT("The player still has no combat target"), Fixture.MainCharacter->CombatTarget);
	TestFalse(TEXT("The combat flag stays false"), Fixture.MainCharacter->bHasCombatTarget);

	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_CombatTargetKeptForOtherEnemy, "FirstCplusProject.Tests.MainCharacter.CombatTarget.KeptForOtherEnemy",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_CombatTargetKeptForOtherEnemy::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy) || !TestNotNull(TEXT("MainCharacter spawned"), Fixture.MainCharacter))
	{
		Fixture.Destroy();
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AEnemy* OtherEnemy = Fixture.World->SpawnActor<AEnemy>(FVector(-500.f, 0.f, 0.f), FRotator::ZeroRotator, SpawnParams);
	if (!TestNotNull(TEXT("Second enemy spawned"), OtherEnemy))
	{
		Fixture.Destroy();
		return false;
	}

	// The player is fighting the second enemy while the first enemy loses sight of them.
	Fixture.MainCharacter->SetCombatTarget(OtherEnemy);
	Fixture.MainCharacter->SetHasCombatTarget(true);

	Fixture.Enemy->AgroSphereOnOverlapEnd(nullptr, Fixture.MainCharacter, nullptr, 0);

	TestEqual(TEXT("Another enemy's target is not cleared"), Fixture.MainCharacter->CombatTarget, OtherEnemy);
	TestTrue(TEXT("The player still has a combat target"), Fixture.MainCharacter->bHasCombatTarget);

	Fixture.Destroy();
	return true;
}
