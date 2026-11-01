#include "CombatTestHelpers.h"
#include "TakeDamageListener.h"
#include "FirstCplusProject/GameplayActors/Enemy.h"
#include "FirstCplusProject/GameplayActors/MainCharacter.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#include "UObject/StrongObjectPtr.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_EnemyDamageFiresOnTakeAnyDamage, "FirstCplusProject.Tests.Enemy.TakeDamage.FiresOnTakeAnyDamage",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_EnemyDamageFiresOnTakeAnyDamage::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld(true);
	if (!TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy))
	{
		Fixture.Destroy();
		return false;
	}

	TStrongObjectPtr<UTakeDamageListener> Listener(NewObject<UTakeDamageListener>());
	Fixture.Enemy->OnTakeAnyDamage.AddDynamic(Listener.Get(), &UTakeDamageListener::OnDamage);

	const float HealthBefore = Fixture.Enemy->Health;
	const float Applied = UGameplayStatics::ApplyDamage(Fixture.Enemy, 10.f, nullptr, nullptr, UDamageType::StaticClass());

	TestTrue(TEXT("OnTakeAnyDamage fires when damage is applied"), Listener->bFired);
	TestEqual(TEXT("The damage reported as applied is the damage dealt"), Applied, 10.f);
	TestEqual(TEXT("Health drops by the applied damage"), Fixture.Enemy->Health, HealthBefore - Applied);

	Fixture.Enemy->OnTakeAnyDamage.RemoveDynamic(Listener.Get(), &UTakeDamageListener::OnDamage);
	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_MainCharacterDamageFiresOnTakeAnyDamage, "FirstCplusProject.Tests.MainCharacter.TakeDamage.FiresOnTakeAnyDamage",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_MainCharacterDamageFiresOnTakeAnyDamage::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld(true);
	if (!TestNotNull(TEXT("MainCharacter spawned"), Fixture.MainCharacter))
	{
		Fixture.Destroy();
		return false;
	}

	TStrongObjectPtr<UTakeDamageListener> Listener(NewObject<UTakeDamageListener>());
	Fixture.MainCharacter->OnTakeAnyDamage.AddDynamic(Listener.Get(), &UTakeDamageListener::OnDamage);

	const float HealthBefore = Fixture.MainCharacter->Health;
	const float Applied = UGameplayStatics::ApplyDamage(Fixture.MainCharacter, 10.f, nullptr, nullptr, UDamageType::StaticClass());

	TestTrue(TEXT("OnTakeAnyDamage fires when damage is applied"), Listener->bFired);
	TestEqual(TEXT("The damage reported as applied is the damage dealt"), Applied, 10.f);
	TestEqual(TEXT("Health drops by the applied damage"), Fixture.MainCharacter->Health, HealthBefore - Applied);

	Fixture.MainCharacter->OnTakeAnyDamage.RemoveDynamic(Listener.Get(), &UTakeDamageListener::OnDamage);
	Fixture.Destroy();
	return true;
}
