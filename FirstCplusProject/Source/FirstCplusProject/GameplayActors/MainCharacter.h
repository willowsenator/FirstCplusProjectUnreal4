// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "MainCharacter.generated.h"

UCLASS()
class FIRSTCPLUSPROJECT_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

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
	UInputAction* InputJump;

	/*** Stop action for jumping */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enhanced Input")
	UInputAction* InputStopJump;

	/** PlayerStats */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Stats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Player Stats")
	int32 Coins;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
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
	void Jump(const FInputActionValue& Value);

	/** Called via input to stop the character from jumping
	 * @param Value This is the input value for stopping the character from jumping
	 */
	void StopJumping(const FInputActionValue& Value);

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
