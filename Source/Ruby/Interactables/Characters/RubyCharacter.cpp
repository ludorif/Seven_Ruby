// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "RubyCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HudGameComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Materials/Material.h"
#include "Ruby/UnrealMainClasses/RubyPlayerController.h"
#include "Ruby/Helpers/RubyLibrary.h"
#include "Components/WidgetComponent.h"
#include "Ruby/UI/LifeUserWidget.h"
#include "Ruby/Interactables/Buildings/Building.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h"

ARubyCharacter::ARubyCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	SphereComponent = CreateDefaultSubobject<UStaticMeshComponent>("SphereComponent");
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetRelativeLocation(FVector(0, 0, 150));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	SphereComponent->SetStaticMesh(SphereMeshAsset.Object);
	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bAllowTickBeforeBeginPlay = false;	
	LifeWidget = CreateDefaultSubobject<UWidgetComponent>("LifeWidget");
	LifeWidget->SetupAttachment(RootComponent);	
	InitLifeWidget(LifeWidget);
	SelectWidget = CreateDefaultSubobject<UWidgetComponent>("SelectWidget");
	SelectWidget->SetupAttachment(RootComponent);
	InitSelectWidget(SelectWidget);

	ACharacter::SetReplicateMovement(true);
}
void ARubyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARubyCharacter, LifePercent);
}
void ARubyCharacter::Init(const uint8 NewTeamId)
{
	Multicast_Init(NewTeamId);
}
void ARubyCharacter::Multicast_Init_Implementation(const uint8 NewTeamId)
{
	TeamId = NewTeamId;
	CurrentLife = TotalLife;
	
	if (Role == ROLE_Authority)
	{
		SpawnDefaultController();
	}
	else
	{
		const FString TeamIdStr = FString::FromInt(TeamId);
		const FString MaterialPath = URubyLibrary::GetTeamMaterialPath(*TeamIdStr);
		UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
		SphereComponent->SetMaterial(0, Material);
		ChangeActionState(EActionState::AS_Idle);
	}
}
void ARubyCharacter::SetMovingTarget(const FVector& NewTargetLocation)
{
	Target = nullptr;
	TargetLocation = FVector(
		NewTargetLocation.X,
		NewTargetLocation.Y,
		GetActorLocation().Z);
	
	ChangeActionState(EActionState::AS_Walking);
	GetController()->StopMovement();
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), TargetLocation);
}
void ARubyCharacter::SetMovingTarget(AActor* NewTarget)
{
	if (NewTarget == nullptr) return; 
	SetMovingTarget(NewTarget->GetActorLocation());
	Target = NewTarget;
}
void ARubyCharacter::StopMovingCustom()
{
	GetCharacterMovement()->StopActiveMovement();
	LastVelocity = 0.0f;

	if (Target != nullptr)
	{
		ChangeActionState(EActionState::AS_Interacting);		
	}
	else
	{
		ChangeActionState(EActionState::AS_Idle);
	}
}
void ARubyCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	if (HasAuthority() && CurrentLife > 0)
	{
		if (CurrentState == EActionState::AS_Walking)
		{
			if (bIsSleeping == 1)
			{
				bIsSleeping = -1;
			}
			const FVector TempTargetLocation = FVector(TargetLocation.X, TargetLocation.Y, GetActorLocation().Z);

			const float Tolerance = 1.0f;
			const float CurrentVelocity = GetCharacterMovement()->Velocity.Size();

			if ((FVector::Dist(GetActorLocation(), TempTargetLocation) < 300) ||
				(LastVelocity > Tolerance&& CurrentVelocity < Tolerance))
			{
				StopMovingCustom();
			}
			else
			{
				LastVelocity = CurrentVelocity;

				FRotator Direction = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
				Direction = FRotator(0, Direction.Yaw - 90, 0);
				SetActorRotation(Direction);
			}
		}
		else if (CurrentState == EActionState::AS_Interacting)
		{
			if (IInteractableInterface* TargetInterface = Cast<IInteractableInterface>(Target))
			{
				const bool bShouldStop = TargetInterface->Interact(this);

				if (bShouldStop)
				{
					UClass* OldTargetClass = Target->GetClass();

					ChangeActionState(EActionState::AS_Idle);
					if (TargetInterface->CurrentLife <= 0)
					{
						Target->Destroy();
					}
					else
					{
						Cast<IInteractableInterface>(Target)->Init(TeamId);
					}
					Target = nullptr;

					OnInteractStoppingBP(OldTargetClass);
				}
			}
		}

		AgePercent.UpdatePercent(-AgingSpeed * DeltaSeconds, CurrentLife);
		TiredPercent.UpdatePercent(bIsSleeping * TiredSpeed * DeltaSeconds, CurrentLife);
		if ((CloseLivreur == nullptr) || (!CloseLivreur->Eat(ResourceToEat, HungerSpeed * DeltaSeconds)))
		{
			HungerPercent.UpdatePercent(-HungerSpeed * DeltaSeconds, CurrentLife);
		}
		if (CurrentLife <= 0)
		{
			Destroy();
		}
		else
		{
			LifePercent = (static_cast<float>(CurrentLife) / TotalLife);

			if (RubyPlayerController != nullptr)
			{
				RubyPlayerController->Client_UpdateHudValues(HungerPercent.GetPercent(), TiredPercent.GetPercent(), AgePercent.GetPercent(), ResourceToEat, ResourceCount);
			}
		}
	}
}
bool ARubyCharacter::Eat(const EResourceType ResourceType, const float EatCount)
{
	if ((ResourceType == ResourceToEat) && ((ResourceCount - EatCount) > 0))
	{
		ResourceCount -= EatCount;
		return true;
	}
	return false;
}

void ARubyCharacter::OnLifePercentChanged()
{
	if (ULifeUserWidget* LocalLifeWidget = Cast<ULifeUserWidget>(LifeWidget->GetUserWidgetObject()))
	{
		LocalLifeWidget->LifeUpdated(LifePercent);
	}	
}
bool ARubyCharacter::Interact(ARubyCharacter* Interactor)
{
	CurrentLife -= Interactor->Damage;
	
	LifePercent = static_cast<float>(CurrentLife) / TotalLife;

	return CurrentLife <=0;
}
