// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "MainCharacter.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Components/SphereComponent.h"

#include "Enemy.generated.h"


class UParticleSystem;
class USoundCue;
class UBoxComponent;
class UAnimMontage;

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking UMETA(DisplayName = "Attacking"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTCPLUSPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(const EEnemyMovementStatus NewStatus) { EnemyMovementStatus = NewStatus; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	USphereComponent *AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	USphereComponent *CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	AAIController *AIController;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float Health;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float Damage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	UParticleSystem *HitParticles;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	USoundCue* HitSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	USoundCue* SwingSound;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Combat")
	UBoxComponent* CombatCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	UAnimMontage* CombatMontage;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bCanAttack;
	
	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	
	UFUNCTION(BlueprintCallable)
	void Attack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	virtual void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable )
	void MoveToTarget(const AMainCharacter * Target);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AI")
	bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="AI")
	AMainCharacter *CombatTarget;
	
	UFUNCTION(BlueprintCallable)
	void ActivateCollision() const;
	
	UFUNCTION(BlueprintCallable)
	void DeactivateCollision() const;
};
