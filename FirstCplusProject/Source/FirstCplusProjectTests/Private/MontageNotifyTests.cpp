#include "FirstCplusProject/GameplayActors/MainCharacter.h"
#include "Animation/AnimMontage.h"
#include "Misc/AutomationTest.h"
#include "UObject/Package.h"

namespace
{
	UAnimMontage* MakeMontageWithNotifies(const TArray<FName>& NotifyNames)
	{
		UAnimMontage* Montage = NewObject<UAnimMontage>(GetTransientPackage());
		for (const FName& Name : NotifyNames)
		{
			FAnimNotifyEvent& Event = Montage->Notifies.AddDefaulted_GetRef();
			Event.NotifyName = Name;
		}
		return Montage;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_MontageCollisionNotifies, "FirstCplusProject.Tests.MainCharacter.MontageNotifies.CollisionNames",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_MontageCollisionNotifies::RunTest(const FString& Parameters)
{
	TestFalse(TEXT("A null montage has no collision notifies"), AMainCharacter::MontageHasCollisionNotifies(nullptr));

	TestFalse(TEXT("A montage without notifies fails"),
	          AMainCharacter::MontageHasCollisionNotifies(MakeMontageWithNotifies({})));

	TestTrue(TEXT("Names without spaces match"),
	         AMainCharacter::MontageHasCollisionNotifies(MakeMontageWithNotifies({TEXT("ActivateCollision"), TEXT("DeactivateCollision")})));

	TestTrue(TEXT("Names with spaces, as Blueprint notifies are named, match"),
	         AMainCharacter::MontageHasCollisionNotifies(MakeMontageWithNotifies({TEXT("Activate Collision"), TEXT("Deactivate Collision")})));

	TestFalse(TEXT("Only the activate notify fails"),
	          AMainCharacter::MontageHasCollisionNotifies(MakeMontageWithNotifies({TEXT("Activate Collision")})));

	TestFalse(TEXT("Only the deactivate notify fails"),
	          AMainCharacter::MontageHasCollisionNotifies(MakeMontageWithNotifies({TEXT("DeactivateCollision")})));

	TestFalse(TEXT("Unrelated notifies fail"),
	          AMainCharacter::MontageHasCollisionNotifies(MakeMontageWithNotifies({TEXT("End Attacking"), TEXT("Play Swing Sound")})));

	return true;
}
