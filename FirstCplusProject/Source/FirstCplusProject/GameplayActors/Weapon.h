// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

class AMainCharacter;
// Forward declarations for component and sound types used in this header
class UStaticMeshComponent;
class UBoxComponent;
class USoundCue;

UENUM(BlueprintType)
enum class EWeaponState: uint8
{
	Ews_Pickup UMETA(DisplayName = "Pickup"),
	Ews_Equipped UMETA(DisplayName = "Equipped"),
	Ews_Max UMETA(DisplayName = "DefaultMAX")
};
/**
 * 
 */
UCLASS()
class FIRSTCPLUSPROJECT_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();
	EWeaponState WeaponState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon | Combat")
	UBoxComponent* CombatCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Sound")
	USoundCue* OnEquipSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Sound")
	USoundCue* SwingSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Weapon | Particles")
	bool bParticleWeapon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Combat")
	float Damage;
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	void Equip(AMainCharacter* Char);

	FORCEINLINE void SetWeaponState(const EWeaponState NewWeaponState) { WeaponState = NewWeaponState; }
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
	
	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision() const;
	
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision() const;
};
