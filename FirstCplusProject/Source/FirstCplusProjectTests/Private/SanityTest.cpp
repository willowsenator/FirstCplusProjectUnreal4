#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_SanityTest, "FirstCplusProject.Tests.Sanity",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_SanityTest::RunTest(const FString& Parameters)
{
	// Make the test pass by returning true, or fail by returning false.
	TestTrue(TEXT("module loaded"), true);
	return true;
}
