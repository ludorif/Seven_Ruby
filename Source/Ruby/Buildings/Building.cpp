// Fill out your copyright notice in the Description page of Project Settings.
#include "Building.h"
#include "Engine/World.h"
#include "UI/RubyUserWidget.h"
#include "UI/LifeUserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/BoxComponent.h"
#include "UnrealMainClasses/RubyPlayerController.h"
#include "Characters/RubyCharacter.h"
#include "ConstructorHelpers.h"
#include "Helpers/RubyLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UnrealNetwork.h"
// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
    AActor::SetReplicateMovement(true);
	SetActorEnableCollision(false);
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	SpawnPoint = CreateDefaultSubobject<USceneComponent>("SpawnPoint");
	SpawnPoint->SetupAttachment(RootComponent);

	ChildActorComponent = CreateDefaultSubobject<UChildActorComponent>("ChildActorComponent");
	ChildActorComponent->SetupAttachment(RootComponent);
	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(ChildActorComponent);

	LifeWidget = CreateDefaultSubobject<UWidgetComponent>("LifeWidget");
	LifeWidget->SetupAttachment(RootComponent);
	InitLifeWidget(LifeWidget);
	SelectWidget = CreateDefaultSubobject<UWidgetComponent>("SelectWidget");
	SelectWidget->SetupAttachment(RootComponent);
	InitSelectWidget(SelectWidget);

	SpawnDestinationSphere = CreateDefaultSubobject<UStaticMeshComponent>("SphereComponent");
	SpawnDestinationSphere->SetupAttachment(RootComponent);
	SpawnDestinationSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	SpawnDestinationSphere->SetStaticMesh(SphereMeshAsset.Object);
	SpawnDestinationSphere->bHiddenInGame = true;

	SpawnDestinationSphere->SetIsReplicated(true);
}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABuilding, bIsBuilt);
}

void ABuilding::Init(const uint8 NewTeamId)
{
	const uint16 DestinationOffset = 500;
	SetSpawnDestination(GetActorLocation() + (GetActorRightVector() * DestinationOffset));
	Multicast_Init(NewTeamId);
}

void ABuilding::Select(ARubyPlayerController* PlayerController)
{
	IInteractableInterface::Select(PlayerController);
	Multicast_ChangeSpawnDestinationVisibility(true);
}
void ABuilding::Unselect(ARubyPlayerController* PlayerController)
{
	IInteractableInterface::Unselect(PlayerController);
	Multicast_ChangeSpawnDestinationVisibility(false);
}
void ABuilding::Multicast_Init_Implementation(const uint8 NewTeamId)
{
	SetActorEnableCollision(true);
	TeamId = NewTeamId;
	if (Role != ROLE_Authority)
	{
		const FString TeamIdStr = FString::FromInt(TeamId);
		const FString MaterialPath = URubyLibrary::GetTeamMaterialPath(*TeamIdStr);
		UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
		//ChildActorComponent->SetMaterial(0, Material);
	}
}
bool ABuilding::Server_SetPosition_Validate(const FVector_NetQuantize& NewLocation)
{
	return true;
}
void ABuilding::Server_SetPosition_Implementation(const FVector_NetQuantize& NewLocation)
{
	SetActorLocation(NewLocation);
}

bool ABuilding::Interact(ARubyCharacter* Interactor)
{
	bool bShouldStop = false;
	if(IsSameTeam(Interactor->TeamId))
	{
		CurrentLife += Interactor->Damage;
		if (CurrentLife == TotalLife)
		{
			bIsBuilt = true;
			bShouldStop = true;
		}
	}
	else
	{
		CurrentLife -= Interactor->Damage;
		if (CurrentLife <=0) bShouldStop = true;
	}
	Multicast_LifeUpdated(static_cast<float>(CurrentLife) / TotalLife);

	return bShouldStop;
}
void ABuilding::Multicast_LifeUpdated_Implementation(const float LifePercent)
{
	ULifeUserWidget* LocalLifeWidget = Cast<ULifeUserWidget>(LifeWidget->GetUserWidgetObject());
	LocalLifeWidget->LifeUpdated(LifePercent);
}

void ABuilding::Multicast_ChangeSpawnDestinationVisibility_Implementation(const bool NewVisibility)
{
	SpawnDestinationSphere->SetHiddenInGame(!NewVisibility);
}

void ABuilding::SetSpawnDestination(const FVector& NewSpawnDestination) const
{
	SpawnDestinationSphere->SetWorldLocation(NewSpawnDestination);
}


