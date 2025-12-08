// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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

	// Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(48.f, 70.f);

	// Don't rotate when the controller rotates
	// Let that just affect the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 840.0f, 0.0f); // ... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Set base rates for turning and looking up
	BaseTurnRate = 65.0f;
	BaseLookUpRate = 65.0f;

	// Player stats
	MaxHealth = 100.0f;
	Health = 65.0f;
	MaxStamina = 350.0f;
	Stamina = 120.0f;
	Coins = 0;
	MaxCoins = 99999;

	RunningSpeed = 650.0f;
	SprintingSpeed = 950.0f;

	// Initialize enums
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.0f;

	// Initialize boolean flags
	bIsDead = false;
	bAttacking = false;
	bSprinting = false;
	bLMB = false;
}

void AMainCharacter::ShowPickupLocations()
{
	for (auto const PickupLocation : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, PickupLocation, 25.0f, 40, FLinearColor::Green, 5.f, .2f);
	}
}

void AMainCharacter::DecreaseHealth(const float Amount)
{
	if (Health - Amount <= 0.0f)
	{
		Health = 0.0f;
		Die();
	}
	else
	{
		Health -= Amount;
	}
}

void AMainCharacter::Die()
{
	if (bIsDead) return;  // Prevent multiple deaths
	
	bIsDead = true;
	
	// Disable player input
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		DisableInput(PC);
	}
	
	// Stop movement
	GetCharacterMovement()->DisableMovement();
	
	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// TODO: Play death animation
	// TODO: Trigger respawn timer
	// TODO: Notify GameMode
	
	UE_LOG(LogTemp, Warning, TEXT("Player died at location: %s"), *GetActorLocation().ToString());
}

void AMainCharacter::IncrementCoins(const int32 Amount)
{
	if (Coins + Amount <= MaxCoins)
	{
		Coins += Amount;
	}
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const auto DeltaStamina = StaminaDrainRate * DeltaTime;

	if (bSprinting)
	{
		HandleSprinting(DeltaStamina);
	}
	else
	{
		HandleNotSprinting(DeltaStamina);
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get player Controller
	const auto* PlayerController = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	auto* Subsystem = ULocalPlayer::GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	// Clear out existing mappings
	Subsystem->ClearAllMappings();

	Subsystem->AddMappingContext(InputMapping, 0);

	auto* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(InputMove, ETriggerEvent::Triggered, this, &AMainCharacter::Move);

	EnhancedInputComponent->BindAction(InputLook, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
	EnhancedInputComponent->BindAction(InputTurnRate, ETriggerEvent::Triggered, this, &AMainCharacter::TurnAtRate);
	EnhancedInputComponent->BindAction(InputLookUpRate, ETriggerEvent::Triggered, this, &AMainCharacter::LookUpAtRate);

	EnhancedInputComponent->BindAction(InputStartJumping, ETriggerEvent::Triggered, this,
	                                   &AMainCharacter::StartJumping);
	EnhancedInputComponent->BindAction(InputStopJumping, ETriggerEvent::Triggered, this, &AMainCharacter::StopJumping);

	EnhancedInputComponent->BindAction(InputStartSprinting, ETriggerEvent::Triggered, this,
	                                   &AMainCharacter::StartSprinting);
	EnhancedInputComponent->BindAction(InputStopSprinting, ETriggerEvent::Triggered, this,
	                                   &AMainCharacter::StopSprinting);

	EnhancedInputComponent->BindAction(InputLMBDown, ETriggerEvent::Triggered, this, &AMainCharacter::LMBDown);
	EnhancedInputComponent->BindAction(InputLMBUp, ETriggerEvent::Triggered, this, &AMainCharacter::LMBUp);
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	const auto MoveValue = Value.Get<FVector2d>();
	if (Controller == nullptr || MoveValue.X == 0.0f && MoveValue.Y == 0.0f || bAttacking)
	{
		return;
	}
	// Find out which way is forward
	const auto Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const auto DirectionX = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const auto DirectionY = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(DirectionX, MoveValue.X);
	AddMovementInput(DirectionY, MoveValue.Y);

	// Rotate character to face direction of movement
	if (const auto DesiredDirection = DirectionX * MoveValue.X + DirectionY * MoveValue.Y; !DesiredDirection.
		IsNearlyZero())
	{
		const auto DesiredRotation = DesiredDirection.Rotation();
		SetActorRotation(FRotator(0.0f, DesiredRotation.Yaw, 0.0f));
	}
}

void AMainCharacter::TurnAtRate(const FInputActionValue& Rate)
{
	auto const RateValue = Rate.Get<float>();
	AddControllerYawInput(RateValue * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LookUpAtRate(const FInputActionValue& Rate)
{
	auto const RateValue = Rate.Get<float>();
	AddControllerPitchInput(RateValue * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	auto const LookValue = Value.Get<FVector2d>();
	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AMainCharacter::StartJumping(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		ACharacter::Jump();
	}
}

void AMainCharacter::StopJumping(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		ACharacter::StopJumping();
	}
}

void AMainCharacter::StartSprinting(const FInputActionValue& Value)
{
	if (Value.Get<bool>() && GetVelocity().Size() > 0.0f && Stamina > 0.0f)
	{
		bSprinting = true;
	}
}

void AMainCharacter::StopSprinting(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		bSprinting = false;
	}
}

void AMainCharacter::LMBDown(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		bLMB = true;
		if (ActiveOverlappingItem)
		{
			AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
			if (Weapon)
			{
				Weapon->Equip(this);
				ActiveOverlappingItem = nullptr; // Clear the active overlapping item after equipping
			}
		}
		else if (EquippedWeapon)
		{
			Attack();
		}
	}
}

void AMainCharacter::LMBUp(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		bLMB = false;
	}
}


void AMainCharacter::SetMovementStatus(const EMovementStatus NewMovementStatus)
{
	MovementStatus = NewMovementStatus;
	UpdateMovementSpeed();
}

void AMainCharacter::UpdateMovementSpeed() const
{
	GetCharacterMovement()->MaxWalkSpeed = (MovementStatus == EMovementStatus::EMS_Sprinting)
		                                       ? SprintingSpeed
		                                       : RunningSpeed;
}

void AMainCharacter::HandleSprinting(const float DeltaStamina)
{
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (Stamina - DeltaStamina <= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
		}
		Stamina -= DeltaStamina;
		SetMovementStatus(EMovementStatus::EMS_Sprinting);
		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (Stamina - DeltaStamina <= 0.f)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
			Stamina = 0.f;
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		else
		{
			Stamina -= DeltaStamina;
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		break;

	case EStaminaStatus::ESS_Exhausted:
		Stamina = 0.f;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	default:
		break;
	}
}

void AMainCharacter::HandleNotSprinting(const float DeltaStamina)
{
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (Stamina + DeltaStamina >= MaxStamina)
		{
			Stamina = MaxStamina;
		}
		else
		{
			Stamina += DeltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_BelowMinimum:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		Stamina += DeltaStamina;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_Exhausted:
		SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
		Stamina += DeltaStamina;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:
		if (Stamina + DeltaStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		Stamina += DeltaStamina;
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	default:
		break;
	}
}

void AMainCharacter::Attack()
{
	if (!EquippedWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot attack - no weapon equipped"));
		return;
	}
	
	if (bAttacking) return;

	bAttacking = true;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance(); AnimInstance && CombatMontage)
	{
		switch (FMath::RandRange(0, 1))
		{
		case 0:
			AnimInstance->Montage_Play(CombatMontage, 2.2f);
			AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
			break;
		case 1:
			AnimInstance->Montage_Play(CombatMontage, 1.8f);
			AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
			break;
		default:
			break;
		}
	}
}

void AMainCharacter::AttackEnd()
{
	bAttacking = false;

	if (bLMB)
	{
		Attack();
	}
}
