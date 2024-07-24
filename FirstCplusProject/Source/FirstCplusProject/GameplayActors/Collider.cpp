// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ACollider::ACollider()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());

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
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
}

void ACollider::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2d MoveValue = Value.Get<FVector2d>();
		
		// Forward/Backward direction
		if (MoveValue.Y != 0.f)
		{
			AddMovementInput(MoveValue.Y * FVector::ForwardVector);
		} 

		// Right/Left direction
		if(MoveValue.X != 0.f)
		{
			AddMovementInput(MoveValue.X * FVector::RightVector);
		}
	}
}
