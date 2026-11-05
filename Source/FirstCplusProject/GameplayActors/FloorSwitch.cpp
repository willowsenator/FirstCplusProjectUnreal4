// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerBox->SetBoxExtent(FVector(63.f, 63.f, 32.f));
	
	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	Tree = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tree"));
	Tree->SetupAttachment(GetRootComponent());

	SwitchTime = 2.f;
	bCharacterOnSwitch = false;
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);

	InitialSwitchLocation = FloorSwitch->GetComponentLocation();
	InitialTreeLocation = Tree->GetComponentLocation();
}

// Called every frame
void AFloorSwitch::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	HandleOverlap(true);
}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HandleOverlap(false);
}

void AFloorSwitch::HandleOverlap(const bool bBeginOverlap)
{
	if (bBeginOverlap)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
		if(!bCharacterOnSwitch)
		{
			bCharacterOnSwitch = true;
		}
		MoveUpTree();
		MoveDownFloorSwitch();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap End"));
		if (bCharacterOnSwitch)
		{
			bCharacterOnSwitch = false;
		}
		GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwitch::RestoreTreeAndSwitchPosition, SwitchTime);
	}
}

void AFloorSwitch::UpdateTreeLocation(const float Z) const
{
	FVector NewLocation = InitialTreeLocation;
	NewLocation.Z += Z;
	Tree->SetWorldLocation(NewLocation);
}

void AFloorSwitch::UpdateFloorSwitchLocation(const float Z) const
{
	FVector NewLocation = InitialSwitchLocation;
	NewLocation.Z += Z;
	FloorSwitch->SetWorldLocation(NewLocation);
}

void AFloorSwitch::RestoreTreeAndSwitchPosition()
{
	if(!bCharacterOnSwitch)
	{
		MoveDownTree();
		MoveUpFloorSwitch();
	}
}





