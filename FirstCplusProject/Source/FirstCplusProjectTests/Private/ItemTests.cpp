#include "FirstCplusProject/GameplayActors/Item.h"
#include "Components/SphereComponent.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_ItemPlacedInstancesUnbound, "FirstCplusProject.Tests.Item.PlacedInstances.NoPreBoundOverlap",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_ItemPlacedInstancesUnbound::RunTest(const FString& Parameters)
{
	// Guards the saved map data: a binding serialized into a placed item makes the AddDynamic in AItem::BeginPlay a duplicate.
	// The cause is removed from the data rather than tolerated in code, so this checks the map and not AItem::BeginPlay itself.
	const UWorld* World = LoadObject<UWorld>(nullptr, TEXT("/Game/GameProject/Map/Main_Map_Mediterranean_Coast.Main_Map_Mediterranean_Coast"));
	if (!TestNotNull(TEXT("Mediterranean coast map loads"), World))
	{
		return false;
	}

	int32 ItemsChecked = 0;
	for (const ULevel* Level : World->GetLevels())
	{
		for (AActor* Actor : Level->Actors)
		{
			const AItem* Item = Cast<AItem>(Actor);
			if (!Item)
			{
				continue;
			}

			++ItemsChecked;
			if (!TestNotNull(*FString::Printf(TEXT("%s has a collision volume"), *Item->GetPathName()), Item->CollisionVolume))
			{
				continue;
			}

			TestFalse(*FString::Printf(TEXT("%s has no overlap-begin binding before BeginPlay"), *Item->GetPathName()),
			          Item->CollisionVolume->OnComponentBeginOverlap.IsAlreadyBound(Item, &AItem::OnOverlapBegin));
			TestFalse(*FString::Printf(TEXT("%s has no overlap-end binding before BeginPlay"), *Item->GetPathName()),
			          Item->CollisionVolume->OnComponentEndOverlap.IsAlreadyBound(Item, &AItem::OnOverlapEnd));
		}
	}

	TestTrue(TEXT("The map contains placed items to check"), ItemsChecked > 0);
	return true;
}
