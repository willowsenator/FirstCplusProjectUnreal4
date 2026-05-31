#include "CombatTestHelpers.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_CombatFixtureRoundTrip, "FirstCplusProject.Tests.CombatFixture.RoundTrip", 
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_CombatFixtureRoundTrip::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	
	TestNotNull(TEXT("World spawned"), Fixture.World);
	TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy);
	TestNotNull(TEXT("MainCharacter spawned"), Fixture.MainCharacter);
	TestNotNull(TEXT("PlayerController spawned"), Fixture.PlayerController);
	
	Fixture.Destroy();
	TestNull(TEXT("World cleared after Destroy"), Fixture.World);
	
	// Make the test pass by returning true, or fail by returning false.
	return true;
}
