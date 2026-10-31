#include "Misc/AutomationTest.h"

#include "Blueprint/UserWidget.h"
#include "FirstCplusProject/GameplayActors/Enemy.h"
#include "FirstCplusProject/GameplayActors/MainCharacter.h"
#include "FirstCplusProject/GameplayActors/MainPlayerController.h"
#include "FirstCplusProject/GameplayActors/Weapon.h"

namespace
{
	template <typename T>
	T* LoadBPDefaultObject(const TCHAR* ClassPath)
	{
		UClass* Class = LoadClass<T>(nullptr, ClassPath);
		return Class ? Cast<T>(Class->GetDefaultObject()) : nullptr;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_EnemyBP_HasRequiredAssetSlots, "FirstCplusProject.Tests.AssetConfig.EnemyBP",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_EnemyBP_HasRequiredAssetSlots::RunTest(const FString& Parameters)
{
	AEnemy* Default = LoadBPDefaultObject<AEnemy>(TEXT("/Game/GameProject/Enemies/Troll/Enemy_BP.Enemy_BP_C"));
	if (!TestNotNull(TEXT("Enemy_BP_C loads"), Default))
	{
		return false;
	}

	TestNotNull(TEXT("DamageTypeClass is set"), Default->DamageTypeClass.Get());
	TestNotNull(TEXT("CombatMontage is set"), Default->CombatMontage);
	TestNotNull(TEXT("HitParticles is set"), Default->HitParticles);
	TestNotNull(TEXT("HitSound is set"), Default->HitSound);
	TestNotNull(TEXT("SwingSound is set"), Default->SwingSound);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_WeaponBP_HasRequiredAssetSlots, "FirstCplusProject.Tests.AssetConfig.WeaponBP",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_WeaponBP_HasRequiredAssetSlots::RunTest(const FString& Parameters)
{
	AWeapon* Default = LoadBPDefaultObject<AWeapon>(TEXT("/Game/GameProject/Blueprints/Weapon_BP.Weapon_BP_C"));
	if (!TestNotNull(TEXT("Weapon_BP_C loads"), Default))
	{
		return false;
	}

	TestNotNull(TEXT("DamageTypeClass is set"), Default->DamageTypeClass.Get());
	TestNotNull(TEXT("SwingSound is set"), Default->SwingSound);
	TestNotNull(TEXT("OnEquipSound is set"), Default->OnEquipSound);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_MainPlayerControllerBP_HasRequiredAssetSlots, "FirstCplusProject.Tests.AssetConfig.MainPlayerControllerBP",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_MainPlayerControllerBP_HasRequiredAssetSlots::RunTest(const FString& Parameters)
{
	AMainPlayerController* Default = LoadBPDefaultObject<AMainPlayerController>(
		TEXT("/Game/GameProject/Blueprints/MainPlayerController_BP.MainPlayerController_BP_C"));
	if (!TestNotNull(TEXT("MainPlayerController_BP_C loads"), Default))
	{
		return false;
	}

	TestNotNull(TEXT("HUDOverlayAsset is set"), Default->HUDOverlayAsset.Get());
	TestNotNull(TEXT("WEnemyHealthBar is set"), Default->WEnemyHealthBar.Get());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_MainCharacterBP_HasRequiredAssetSlots, "FirstCplusProject.Tests.AssetConfig.MainCharacterBP",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_MainCharacterBP_HasRequiredAssetSlots::RunTest(const FString& Parameters)
{
	AMainCharacter* Default = LoadBPDefaultObject<AMainCharacter>(
		TEXT("/Game/GameProject/Character/MyMainCharacter_BP.MyMainCharacter_BP_C"));
	if (!TestNotNull(TEXT("MyMainCharacter_BP_C loads"), Default))
	{
		return false;
	}

	TestNotNull(TEXT("CombatMontage is set"), Default->CombatMontage);
	TestNotNull(TEXT("InputMapping is set"), Default->InputMapping);
	TestNotNull(TEXT("InputMove is set"), Default->InputMove);
	TestNotNull(TEXT("InputTurnRate is set"), Default->InputTurnRate);
	TestNotNull(TEXT("InputLook is set"), Default->InputLook);
	TestNotNull(TEXT("InputLookUpRate is set"), Default->InputLookUpRate);
	TestNotNull(TEXT("InputStartJumping is set"), Default->InputStartJumping);
	TestNotNull(TEXT("InputStopJumping is set"), Default->InputStopJumping);
	TestNotNull(TEXT("InputStartSprinting is set"), Default->InputStartSprinting);
	TestNotNull(TEXT("InputStopSprinting is set"), Default->InputStopSprinting);
	TestNotNull(TEXT("InputLMBDown is set"), Default->InputLMBDown);
	TestNotNull(TEXT("InputLMBUp is set"), Default->InputLMBUp);
	TestNotNull(TEXT("HitParticles is set"), Default->HitParticles);
	TestNotNull(TEXT("HitSound is set"), Default->HitSound);

	return true;
}
