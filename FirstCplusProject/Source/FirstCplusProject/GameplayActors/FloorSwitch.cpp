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

	Tree = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Tree->SetupAttachment(GetRootComponent());
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
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MoveUpTree();
	MoveDownFloorSwitch();
}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	MoveDownTree();
	MoveUpFloorSwitch();
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
	NewLocation.Z -= Z;
	FloorSwitch->SetWorldLocation(NewLocation);
}




