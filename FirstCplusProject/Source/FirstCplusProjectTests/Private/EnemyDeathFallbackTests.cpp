#include "CombatTestHelpers.h"
#include "CoreGlobals.h"
#include "Engine/World.h"
#include "FirstCplusProject/GameplayActors/Enemy.h"
#include "Misc/AutomationTest.h"

namespace
{
	void AdvanceWorld(UWorld* World, float Seconds)
	{
		constexpr float StepSeconds = 0.1f;
		const int32 StepCount = FMath::RoundToInt(Seconds / StepSeconds);
		for (int32 Step = 0; Step < StepCount; ++Step)
		{
			++GFrameCounter;
			World->Tick(LEVELTICK_All, StepSeconds);
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_EnemyDestroyedWithoutNotify, "FirstCplusProject.Tests.Enemy.Death.DestroyedWithoutNotify",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_EnemyDestroyedWithoutNotify::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy))
	{
		Fixture.Destroy();
		return false;
	}

	Fixture.Enemy->DeathDelay = 1.f;
	Fixture.Enemy->DeathEndFallbackDelay = 5.f;
	const float TimingMargin = Fixture.Enemy->DeathDelay * 0.2f;
	Fixture.Enemy->Die();
	AdvanceWorld(Fixture.World, Fixture.Enemy->DeathEndFallbackDelay - TimingMargin);
	TestTrue(TEXT("Enemy still valid before fallback deadline"), IsValid(Fixture.Enemy));
	Fixture.Enemy->Die();
	AdvanceWorld(Fixture.World, 2.f * TimingMargin);
	TestTrue(TEXT("Enemy still valid after fallback while DeathDelay is pending"), IsValid(Fixture.Enemy));
	AdvanceWorld(Fixture.World, Fixture.Enemy->DeathDelay);
	TestFalse(TEXT("Enemy destroyed without a DeathEnd notify, even after repeated Die"), IsValid(Fixture.Enemy));

	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_EnemyNotifyPathDestroysOnce, "FirstCplusProject.Tests.Enemy.Death.NotifyPathDestroysOnce",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_EnemyNotifyPathDestroysOnce::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy))
	{
		Fixture.Destroy();
		return false;
	}

	Fixture.Enemy->DeathDelay = 1.f;
	Fixture.Enemy->Die();
	Fixture.Enemy->DeathEnd();
	AdvanceWorld(Fixture.World, 2.f);
	TestFalse(TEXT("Notify destroys enemy after DeathDelay"), IsValid(Fixture.Enemy));
	AdvanceWorld(Fixture.World, 5.f);
	TestFalse(TEXT("Enemy remains destroyed after fallback time elapses"), IsValid(Fixture.Enemy));

	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_EnemySecondDeathEndIsIgnored, "FirstCplusProject.Tests.Enemy.Death.SecondDeathEndIsIgnored",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_EnemySecondDeathEndIsIgnored::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy))
	{
		Fixture.Destroy();
		return false;
	}

	Fixture.Enemy->DeathDelay = 1.f;
	Fixture.Enemy->DeathEnd();
	AdvanceWorld(Fixture.World, 0.6f);
	TestTrue(TEXT("Enemy still valid before original destroy deadline"), IsValid(Fixture.Enemy));
	Fixture.Enemy->DeathEnd();
	AdvanceWorld(Fixture.World, 0.6f);
	TestFalse(TEXT("Second DeathEnd does not postpone original destroy deadline"), IsValid(Fixture.Enemy));

	Fixture.Destroy();
	return true;
}
