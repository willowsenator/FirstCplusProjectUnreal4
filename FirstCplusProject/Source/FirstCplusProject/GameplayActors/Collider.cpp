// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "ColliderMovementComponent.h"
#include "Logging/LogMacros.h"

// Sets default values
ACollider::ACollider()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);

	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
	MeshComponentAsset(TEXT("/Script/Engine.StaticMesh'/Game/InfinityBladeAdversaries/Maps/LevelContent/Architecture/SM_CycRoom_01.SM_CycRoom_01'"));
	if(MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
		MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
		MeshComponent->SetWorldScale3D(FVector(0.2f));
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;
	

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	OurMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("OurMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	CameraInput = FVector2d(0.0f, 0.0f);
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	SetActorRotation(NewRotation);

	FRotator NewSpringArmRotation = SpringArm->GetComponentRotation();
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -80.0f, -15.0f);
	SpringArm->SetWorldRotation(NewSpringArmRotation);
}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get player Controller
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	// Clear out existing mappings
	Subsystem->ClearAllMappings();

	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(InputMove, ETriggerEvent::Triggered, this, &ACollider::Move);

	EnhancedInputComponent->BindAction(InputCameraPitch, ETriggerEvent::Triggered, this, &ACollider::CameraPitch);
	EnhancedInputComponent->BindAction(InputCameraYaw, ETriggerEvent::Triggered, this, &ACollider::CameraYaw);
}

UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return OurMovementComponent;
}

void ACollider::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2d MoveValue = Value.Get<FVector2d>();
		const auto Right = GetActorRightVector();
		const auto Forward = GetActorForwardVector();
		// Forward/Backward direction
		if (MoveValue.X != 0.f && OurMovementComponent)
		{
			OurMovementComponent->AddInputVector(MoveValue.X * Forward);
		} 

		// Right/Left direction
		if(MoveValue.Y != 0.f && OurMovementComponent) 
		{
			OurMovementComponent->AddInputVector(MoveValue.Y * Right);
		}
	}
}

void ACollider::CameraPitch(const FInputActionValue& Value)
{
	if(Controller != nullptr)
	{
		const float PitchValue = Value.Get<float>();
		CameraInput.Y = PitchValue;
	}
}

void ACollider::CameraYaw(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const float YawValue = Value.Get<float>();
		CameraInput.X = YawValue;
	}
}
