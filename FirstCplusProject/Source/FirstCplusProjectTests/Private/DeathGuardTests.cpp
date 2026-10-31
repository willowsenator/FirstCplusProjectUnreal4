#include "CombatTestHelpers.h"
#include "FirstCplusProject/GameplayActors/Enemy.h"
#include "FirstCplusProject/GameplayActors/MainCharacter.h"
#include "FirstCplusProject/GameplayActors/Weapon.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/AutomationTest.h"

namespace
{
	constexpr float LethalDamage = 100000.f;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_EnemyStaysDeadAfterStatusChange, "FirstCplusProject.Tests.Enemy.Die.StaysDeadAfterStatusChange",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_EnemyStaysDeadAfterStatusChange::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy))
	{
		Fixture.Destroy();
		return false;
	}

	Fixture.Enemy->DecreaseHealth(LethalDamage);
	TestFalse(TEXT("Enemy is dead after lethal damage"), Fixture.Enemy->Alive());

	// Overlap-end and attack-end handlers move a living enemy back to idle; a corpse must stay dead.
	Fixture.Enemy->SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
	TestFalse(TEXT("Dead enemy stays dead after an idle transition"), Fixture.Enemy->Alive());

	Fixture.Enemy->SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	TestFalse(TEXT("Dead enemy stays dead after a move transition"), Fixture.Enemy->Alive());

	TestEqual(TEXT("Dead enemy cannot move"), Fixture.Enemy->GetCharacterMovement()->MovementMode.GetValue(), MOVE_None);

	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_DeadEnemyTakesNoDamage, "FirstCplusProject.Tests.Enemy.TakeDamage.IgnoredWhenDead",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_DeadEnemyTakesNoDamage::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("Enemy spawned"), Fixture.Enemy))
	{
		Fixture.Destroy();
		return false;
	}

	Fixture.Enemy->DecreaseHealth(LethalDamage);

	const float Applied = Fixture.Enemy->TakeDamage(5.f, FDamageEvent(), nullptr, nullptr);

	TestEqual(TEXT("A dead enemy reports no damage applied"), Applied, 0.f);
	TestEqual(TEXT("Health stays at zero"), Fixture.Enemy->Health, 0.f);
	TestFalse(TEXT("Enemy is still dead"), Fixture.Enemy->Alive());

	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_DeadPlayerCannotAttack, "FirstCplusProject.Tests.MainCharacter.Die.CannotAttackWhileHoldingLMB",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_DeadPlayerCannotAttack::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("MainCharacter spawned"), Fixture.MainCharacter))
	{
		Fixture.Destroy();
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AWeapon* Weapon = Fixture.World->SpawnActor<AWeapon>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (!TestNotNull(TEXT("Weapon spawned"), Weapon))
	{
		Fixture.Destroy();
		return false;
	}
	Fixture.MainCharacter->SetEquippedWeapon(Weapon);

	// The player dies mid-swing while still holding the attack button.
	Fixture.MainCharacter->bLMB = true;
	Fixture.MainCharacter->bAttacking = true;
	Fixture.MainCharacter->Die();

	TestFalse(TEXT("Dying releases the attack button state"), Fixture.MainCharacter->bLMB);
	TestFalse(TEXT("Dying ends the swing in progress"), Fixture.MainCharacter->bAttacking);

	// The attack timer or the montage end then reports the swing finished.
	Fixture.MainCharacter->AttackEnd();
	TestFalse(TEXT("A finished swing does not chain a new attack after death"), Fixture.MainCharacter->bAttacking);

	Fixture.Destroy();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFirstCplusProjectTests_DeadPlayerAttackIsRefused, "FirstCplusProject.Tests.MainCharacter.Attack.RefusedWhenDead",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFirstCplusProjectTests_DeadPlayerAttackIsRefused::RunTest(const FString& Parameters)
{
	FCombatWorldFixture Fixture = FCombatWorldFixture::MakeCombatWorld();
	if (!TestNotNull(TEXT("MainCharacter spawned"), Fixture.MainCharacter))
	{
		Fixture.Destroy();
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AWeapon* Weapon = Fixture.World->SpawnActor<AWeapon>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (!TestNotNull(TEXT("Weapon spawned"), Weapon))
	{
		Fixture.Destroy();
		return false;
	}
	Fixture.MainCharacter->SetEquippedWeapon(Weapon);
	Fixture.MainCharacter->Die();

	Fixture.MainCharacter->Attack();

	TestFalse(TEXT("A dead player with a weapon does not start an attack"), Fixture.MainCharacter->bAttacking);
	TestFalse(TEXT("A dead player does not turn toward a target"), Fixture.MainCharacter->bInterpToEnemy);

	Fixture.Destroy();
	return true;
}
