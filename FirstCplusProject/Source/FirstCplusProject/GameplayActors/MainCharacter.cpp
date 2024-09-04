// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Set base rates for turning and looking up
	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get player Controller
	const auto *PlayerController = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	auto *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	// Clear out existing mappings
	Subsystem->ClearAllMappings();

	Subsystem->AddMappingContext(InputMapping, 0);

	auto *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(InputMove, ETriggerEvent::Triggered, this, &AMainCharacter::Move);

	EnhancedInputComponent->BindAction(InputLook, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
	EnhancedInputComponent->BindAction(InputTurnRate, ETriggerEvent::Triggered, this, &AMainCharacter::TurnAtRate);
	EnhancedInputComponent->BindAction(InputLookUpRate, ETriggerEvent::Triggered, this, &AMainCharacter::LookUpAtRate);

	EnhancedInputComponent->BindAction(InputJump, ETriggerEvent::Triggered, this, &AMainCharacter::Jump);
	EnhancedInputComponent->BindAction(InputStopJump, ETriggerEvent::Triggered, this, &AMainCharacter::StopJumping);
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	
	if(const FVector2d MoveValue = Value.Get<FVector2d>(); Controller != nullptr && MoveValue.X != 0.0f
		|| MoveValue.Y != 0.0f)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(DirectionX, MoveValue.X);
		AddMovementInput(DirectionY, MoveValue.Y);
	}
}

void AMainCharacter::TurnAtRate(const FInputActionValue& Rate){
	auto const RateValue = Rate.Get<float>();
	AddControllerYawInput(RateValue * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(const FInputActionValue& Rate){
	auto const RateValue = Rate.Get<float>();
    AddControllerPitchInput(RateValue * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::Look(const FInputActionValue& Value){
	auto const LookValue = Value.Get<FVector2d>();
	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AMainCharacter::Jump(const FInputActionValue& Value){
	if(Value.Get<bool>()){
		ACharacter::Jump();
	}
}

void AMainCharacter::StopJumping(const FInputActionValue& Value){
	if(Value.Get<bool>()){
		ACharacter::StopJumping();
	}
}