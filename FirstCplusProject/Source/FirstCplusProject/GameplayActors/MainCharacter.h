// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Weapon.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
    ESS_Normal UMETA(DisplayName = "Normal"),
    ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
    ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),
	
    ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class FIRSTCPLUSPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();
	
	TArray<FVector> PickupLocations;
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(const EStaminaStatus NewStaminaStatus) { StaminaStatus = NewStaminaStatus; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;
	
	void SetMovementStatus(EMovementStatus NewMovementStatus);
	void UpdateMovementSpeed() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bSprinting;

	bool bLMB;

	/** Camera boom positioning the camera behind the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivate = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivate = "true"))
	class UCameraComponent* FollowCamera;

	/** Turn at rate for turning the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	
	/** Base Look up at rate for turning the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;
	
	/** Mapping context for enhanced input */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	class UInputMappingContext* InputMapping;

	/*** Input action for moving the character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputMove;

	/*** Input action for turning at rate the character */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputTurnRate;
	
	/*** Input action for looking up and down and turning the character*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputLook;

	/*** Input action for looking up and down at rate */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputLookUpRate;

	/*** Input action for jumping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputStartJumping;

	/*** Stop action for jumping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputStopJumping;

	/*** Start action for sprinting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputStartSprinting;

	/*** Stop action for sprinting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputStopSprinting;

	/** LMB action for attacking and collecting items*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputLMBDown;

	/** LMB action for stop from attacking and collecting items*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputLMBUp;
	
	/** PlayerStats */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	int32 Coins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	int32 MaxCoins;

	void DecreaseHealth(float Amount);

	static void Die();
	void IncrementCoins(int32 Amount);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapons")
	AWeapon *EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	AItem *ActiveOverlappingItem;

	FORCEINLINE void SetEquippedWeapon(AWeapon* NewWeapon) { EquippedWeapon = NewWeapon; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* NewItem) { ActiveOverlappingItem = NewItem; }
	
private:
	void Move(const FInputActionValue& Value);

	/** Called via input to and turn the character at rates
	* @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	*/
	void TurnAtRate(const FInputActionValue& Rate);
	
	/** Called via input to look up/down the character at rates
    * @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
    */
	void LookUpAtRate(const FInputActionValue& Rate);
	
	/** Called via input to look up/down and turn left/right the character
	 * @param Value This is the input value for looking up/down the character
	 */
	void Look(const FInputActionValue& Value);

	/** Called via input to make the character jump
	 * @param Value This is the input value for jumping the character
	 */
	void StartJumping(const FInputActionValue& Value);

	/** Called via input to stop the character from jumping
	 * @param Value This is the input value for stopping the character from jumping
	 */
	void StopJumping(const FInputActionValue& Value);

	/*** Called via input to make the character sprint
     * @param Value This is the input value for sprinting the character
     */
	void StartSprinting(const FInputActionValue& Value);

	/*** Called via input to stop the character from sprinting
     * @param Value This is the input value for stopping the character from sprinting
     */
	void StopSprinting(const FInputActionValue& Value);

	/** Called via input to start attacking or collecting items
	 * @param Value This is the input value for attacking or collecting items
	 */
	void LMBDown(const FInputActionValue& Value);

	/** Called via input to stop attacking or collecting items
	 * @param Value This is the input value for stopping from attacking or collecting items
	 */
	void LMBUp(const FInputActionValue& Value);

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera;} 
	

	void HandleNotSprinting(const float DeltaStamina);
	void HandleSprinting(const float DeltaStamina);
};
