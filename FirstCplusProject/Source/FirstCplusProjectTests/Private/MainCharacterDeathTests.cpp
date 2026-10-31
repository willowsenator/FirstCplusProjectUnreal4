#include "CombatTestHelpers.h"
#include "Components/SkeletalMeshComponent.h"
#include "FirstCplusProject/GameplayActors/MainCharacter.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_DieStopsMotionAndTurning, "FirstCplusProject.Tests.MainCharacter.Die.StopsMotionAndTurning",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_DieStopsMotionAndTurning::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("MainCharacter spawned"), Fixture.MainCharacter))
	{
		Fixture.Destroy();
		return false;
	}

	Fixture.MainCharacter->bInterpToEnemy = true;
	Fixture.MainCharacter->bSprinting = true;

	Fixture.MainCharacter->Die();

	TestTrue(TEXT("bIsDead set"), Fixture.MainCharacter->bIsDead);
	TestFalse(TEXT("Interp to enemy cleared"), Fixture.MainCharacter->bInterpToEnemy);
	TestFalse(TEXT("Sprinting cleared"), Fixture.MainCharacter->bSprinting);

	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_DeathEndFreezesPose, "FirstCplusProject.Tests.MainCharacter.DeathEnd.FreezesPose",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_DeathEndFreezesPose::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("MainCharacter spawned"), Fixture.MainCharacter))
	{
		Fixture.Destroy();
		return false;
	}

	Fixture.MainCharacter->DeathEnd();

	const USkeletalMeshComponent* MeshComp = Fixture.MainCharacter->GetMesh();
	if (TestNotNull(TEXT("Mesh present"), MeshComp))
	{
		TestTrue(TEXT("Animations paused"), MeshComp->bPauseAnims);
		TestTrue(TEXT("Skeleton update disabled"), MeshComp->bNoSkeletonUpdate);
	}

	Fixture.Destroy();
	return true;
}
