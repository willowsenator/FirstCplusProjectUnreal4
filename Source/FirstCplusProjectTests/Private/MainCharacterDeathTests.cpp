#include "CombatTestHelpers.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	UCharacterMovementComponent* Movement = Fixture.MainCharacter->GetCharacterMovement();
	if (!TestNotNull(TEXT("Movement component present"), Movement))
	{
		Fixture.Destroy();
		return false;
	}

	Fixture.MainCharacter->bInterpToEnemy = true;
	Fixture.MainCharacter->bSprinting = true;
	Movement->Velocity = FVector(300.f, 0.f, 0.f);

	Fixture.MainCharacter->Die();
	Fixture.MainCharacter->Die();

	TestTrue(TEXT("bIsDead set"), Fixture.MainCharacter->bIsDead);
	TestFalse(TEXT("Interp to enemy cleared"), Fixture.MainCharacter->bInterpToEnemy);
	TestFalse(TEXT("Sprinting cleared"), Fixture.MainCharacter->bSprinting);
	TestEqual(TEXT("Movement disabled"), Movement->MovementMode.GetValue(), MOVE_None);
	TestTrue(TEXT("Velocity zeroed"), Movement->Velocity.IsNearlyZero());

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

	const USkeletalMeshComponent* MeshComp = Fixture.MainCharacter->GetMesh();
	if (TestNotNull(TEXT("Mesh present"), MeshComp))
	{
		TestFalse(TEXT("Animations running before DeathEnd"), MeshComp->bPauseAnims);
		TestFalse(TEXT("Skeleton updating before DeathEnd"), MeshComp->bNoSkeletonUpdate);

		Fixture.MainCharacter->DeathEnd();

		TestTrue(TEXT("Animations paused"), MeshComp->bPauseAnims);
		TestTrue(TEXT("Skeleton update disabled"), MeshComp->bNoSkeletonUpdate);
	}

	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_CombatMontageHasDeathEndNotify, "FirstCplusProject.Tests.MainCharacter.DeathEnd.MontageNotifyWired",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_CombatMontageHasDeathEndNotify::RunTest(const FString& Parameters)
{
	const UClass* Class = LoadClass<AMainCharacter>(nullptr, TEXT("/Game/GameProject/Character/MyMainCharacter_BP.MyMainCharacter_BP_C"));
	const AMainCharacter* Default = Class ? Cast<AMainCharacter>(Class->GetDefaultObject()) : nullptr;
	if (!TestNotNull(TEXT("MyMainCharacter_BP_C loads"), Default) || !TestNotNull(TEXT("CombatMontage is set"), Default->CombatMontage))
	{
		return false;
	}

	const bool bHasDeathEnd = Default->CombatMontage->Notifies.ContainsByPredicate([](const FAnimNotifyEvent& Event)
	{
		return Event.NotifyName == TEXT("DeathEnd");
	});
	TestTrue(TEXT("CombatMontage has a DeathEnd notify so the dead pose is held"), bHasDeathEnd);

	return true;
}
