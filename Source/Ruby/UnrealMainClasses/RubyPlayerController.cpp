// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#include "RubyPlayerController.h"
#include "RubyPlayerState.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Ruby/Interactables/Characters/RubyCharacter.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Ruby/Interactables/Buildings/Building.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetComponent.h"
#include "Components/Button.h"
#include "Ruby/UI/RubyHUD.h"
#include "Ruby/UI/RubyUserWidget.h"
#include "Ruby/UI/MouseZone.h"
#include "Ruby/Helpers\RubyLibrary.h"
#include "Ruby/Helpers/Request/RequestDeclaration.h"
#include "Ruby/Interactables/InteractableInterface.h"
#include "Ruby/Interactables/Characters/Components/HudGameComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

ARubyPlayerController::ARubyPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	bAutoManageActiveCameraTarget = false;
	static ConstructorHelpers::FClassFinder<UHudGameComponent> HudGameComponentClass(TEXT("Blueprint'/Game/Blueprint/Characters/Components/HudGameComponentBP.HudGameComponentBP_C'"));
	HudGameComponentBP = Cast<UHudGameComponent>(CreateDefaultSubobject(FName("HudGameComponentBP"), HudGameComponentClass.Class, HudGameComponentClass.Class, false, false, false));
	CurrentActors = TArray<AActor*>();
}
void ARubyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARubyPlayerController, BuildingToSpawn);
}
#pragma region INPUTS
void ARubyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction<FBoolDelegate>("LeftClick", IE_Pressed, this, &ARubyPlayerController::LeftClick, true);
	InputComponent->BindAction<FBoolDelegate>("LeftClick", IE_Released, this, &ARubyPlayerController::LeftClick, false);
	
	InputComponent->BindAction("RightClick", IE_Released, this, &ARubyPlayerController::MoveToMouseCursor);
	InputComponent->BindAxis("Zoom", this, &ARubyPlayerController::Zoom);
}

void ARubyPlayerController::LeftClick(const bool bStartClick)
{	
	if (bStartClick)
	{
		FHitResult Hit;
		//Todo : limit channels
		GetHitResultUnderCursorByChannel(TraceTypeQuery_MAX, false, Hit);
		StartClickPosition = Hit.ImpactPoint;
		MouseZone = GetWorld()->SpawnActor<AMouseZone>(AMouseZone::StaticClass(), StartClickPosition, FRotator::ZeroRotator);

		TUniquePtr<FLeftClickedActor> LeftClickedActor = MakeUnique<FLeftClickedActor>(nullptr);
		Server_RequestAction(LeftClickedActor->SerializeRequestData(this));
	}
	else
	{		
		for (AActor* Actor : MouseZone->GetOverlappedActors())
		{
			TUniquePtr<FLeftClickedActor> LeftClickedActor = MakeUnique<FLeftClickedActor>(Actor);
			Server_RequestAction(LeftClickedActor->SerializeRequestData(this));
		}
		
		MouseZone->Destroy();
	}
}

void ARubyPlayerController::Zoom(const float ZoomValue)
{
	if (ZoomValue != 0.0f)
	{
		CameraActor->AddActorWorldOffset(-1 * Cam_Speed * ZoomValue * FVector::UpVector);
	}
}

void ARubyPlayerController::LeftClickedActor(AActor* ClickedActor)
{
	IInteractableInterface* SelectedActorInterface = Cast<IInteractableInterface>(ClickedActor);
	if (SelectedActorInterface != nullptr )
	{
		if (SelectedActorInterface->CurrentLife <= 0 || SelectedActorInterface->TeamId != TeamId)
		{
			return;
		}

		if (IsCharacterSelected() && ClickedActor->IsA(ABuilding::StaticClass()) ||
			!IsCharacterSelected() && ClickedActor->IsA(ACharacter::StaticClass())
			)
		{
			EmptyCurrentActors();
		}

		if((IsCurrentActorsEmpty() || IsCharacterSelected())&&
			!CurrentActors.Contains(ClickedActor))
		{
			CurrentActors.Emplace(ClickedActor);
		}

		Client_ChangeUiVisibility(ClickedActor, true);
		SelectedActorInterface->Select(this);
	}
	else if (BuildingToSpawn != nullptr)
	{
		for (uint8 i = 0; i < CurrentActors.Num(); i++)
		{
			GetCurrent<ARubyCharacter>(i)->SetMovingTarget(BuildingToSpawn);
		}

		BuildingToSpawn->RubyPlayerController = this;
		BuildingToSpawn = nullptr;
	}
	else 
	{
		EmptyCurrentActors();
	}
}

void ARubyPlayerController::UpdateCameraPosition()
{
	const uint8 Move_Screen_Offset = 50;

	float MouseLocationX;
	float MouseLocationY;
	int32 ViewportSizeX;
	int32 ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);

	if (GetMousePosition(MouseLocationX, MouseLocationY))
	{
		FVector Offset = FVector::ZeroVector;
		if (MouseLocationX > ViewportSizeX - Move_Screen_Offset)
		{
			Offset = FVector::RightVector;
		}
		else if (MouseLocationX < Move_Screen_Offset)
		{
			Offset = -FVector::RightVector;
		}

		if (MouseLocationY > ViewportSizeY - Move_Screen_Offset)
		{
			Offset -= FVector::ForwardVector;
		}
		else if (MouseLocationY < Move_Screen_Offset)
		{
			Offset += FVector::ForwardVector;
		}

		CameraActor->AddActorWorldOffset(Cam_Speed  * Offset);
	}
}
#pragma endregion

void ARubyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (CameraActor != nullptr)
	{
		UpdateCameraPosition();
	}

	if (BuildingToSpawn != nullptr)
	{
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_MAX, false, Hit);
		BuildingToSpawn->Server_SetPosition(FVector(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, 0));
	}

	if (MouseZone != nullptr)
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_MAX, false, Hit);

		
		if (MouseZone->GetActorLocation() != Hit.ImpactPoint)
		{
			const uint16 Ratio = 100;
			const float NewXScale = Hit.ImpactPoint.X - StartClickPosition.X;
			const float NewYScale = Hit.ImpactPoint.Y - StartClickPosition.Y;

			FVector NewLocation = Hit.ImpactPoint;
			NewLocation = FVector(NewLocation.X - (NewXScale /2), NewLocation.Y - (NewYScale /2),0);
			MouseZone->SetActorLocation(NewLocation);

			MouseZone->UpdateScale(FVector(UKismetMathLibrary::Abs(NewXScale) / Ratio, UKismetMathLibrary::Abs(NewYScale) / Ratio, 0.1f) );
		}
	}
}



void ARubyPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_MAX, false, Hit);

	if (Hit.GetActor()->IsA(ARubyCharacter::StaticClass()))		
	{
		TUniquePtr<FSetMovingTargetActor> SetMovingTargetActor = MakeUnique<FSetMovingTargetActor>(Hit.GetActor());
		Server_RequestAction(SetMovingTargetActor->SerializeRequestData(this));
	}
	else if (Hit.GetActor()->GetParentActor() != nullptr && Hit.GetActor()->GetParentActor()->IsA(ABuilding::StaticClass()))
	{
		TUniquePtr<FSetMovingTargetActor> SetMovingTargetActor = MakeUnique<FSetMovingTargetActor>(Hit.GetActor()->GetParentActor());
		Server_RequestAction(SetMovingTargetActor->SerializeRequestData(this));
	}
	else if (Hit.bBlockingHit)
	{
		TUniquePtr<FSetMovingTargetPosition> SetMovingTargetPosition = MakeUnique<FSetMovingTargetPosition>(Hit.ImpactPoint);
		Server_RequestAction(SetMovingTargetPosition->SerializeRequestData(this));
	}
}



void ARubyPlayerController::Spawn(TSubclassOf<AActor> ClassToSpawn, const FVector NewSpawnLocation, const uint8 Price)
{
	TUniquePtr<FSpawn> Spawn = MakeUnique<FSpawn>(ClassToSpawn->GetName(), NewSpawnLocation, Price);
	Server_RequestAction(Spawn->SerializeRequestData(this));
}

#pragma region RPC
#pragma region SERVER
bool ARubyPlayerController::Server_Init_Validate(ACameraActor* NewCameraActor, int32 NewPlayerId, TSubclassOf<ARubyCharacter> ClassToSpawn)
{
	return true;
}
void ARubyPlayerController::Server_Init_Implementation(ACameraActor* NewCameraActor, int32 NewPlayerId, TSubclassOf<ARubyCharacter> ClassToSpawn)
{
	CameraActor = NewCameraActor;
	TeamId = NewPlayerId; //todo adapt for more than 2 players
	//Todo clean
	
	const FActorSpawnParameters SpawnInfo;
	FHitResult Hit;
	const FVector Location = NewCameraActor->GetActorLocation();
	const FVector Offset = NewCameraActor->GetActorForwardVector() * 100000;
	GetWorld()->LineTraceSingleByChannel(
		Hit,
		Location,
		Location + Offset,
		ECC_WorldStatic);
	
	UClass* DungeonClass = LoadObject<UClass>(nullptr, *URubyLibrary::GetBuildingPath(TEXT("Dungeon")));
	ABuilding* Dungeon = GetWorld()->SpawnActor<ABuilding>(
		DungeonClass, 
		FVector(Hit.ImpactPoint.X, Hit.ImpactPoint.Y, 0),
		FRotator::MakeFromEuler(FVector(0, 0, 90)),
		SpawnInfo);
	Dungeon->SetOwner(this);
	Dungeon->CurrentLife = Dungeon->TotalLife;

	Dungeon->bIsBuilt = true;
	URubyLibrary::DelayedFunction(
		ONE_SECOND * 3,
		GetWorld(),
		[this, NewCameraActor, Dungeon]
		{
			Client_Init(NewCameraActor);
			Cast<IInteractableInterface>(Dungeon)->Init(TeamId);
		}
	);
}

bool ARubyPlayerController::Server_RequestAction_Validate(const FActionRequest& RequestAction)
{
	return true;
}
void ARubyPlayerController::Server_RequestAction_Implementation(const FActionRequest& RequestAction)
{
	bool bShouldMulticast = false;
	const TSharedRef<FActionRequest> RequestActionCopy = MakeShared<FActionRequest>(RequestAction);

	if (RequestAction.IsSameType(NAME_Sleep))
	{
		for (uint8 i = 0; i < CurrentActors.Num(); i++)
		{
			GetCurrent<ARubyCharacter>(i)->ChangeSleepState();
		}
	}
	else if (RequestAction.IsSameType(NAME_LeftClickedActor))
	{
		const TUniquePtr<FLeftClickedActor> LeftClickedRequest = MakeUnique<FLeftClickedActor>(RequestAction, this);
		LeftClickedActor(LeftClickedRequest->ClickedActor);
	}
	else if (RequestAction.IsSameType(NAME_SetMovingTargetPosition))
	{
		const TUniquePtr<FSetMovingTargetPosition> AssignTarget = MakeUnique<FSetMovingTargetPosition>(RequestAction, this);
		if (IsCharacterSelected())
		{
			for (uint8 i = 0; i < CurrentActors.Num(); i++)
			{
				if (ARubyCharacter* RubyCharacter = GetCurrent<ARubyCharacter>(i))
				{
					const FVector Direction = AssignTarget->TargetPosition - RubyCharacter->GetActorLocation();
					FVector OffsetDirection = UKismetMathLibrary::RotateAngleAxis(Direction, 90.0f, FVector::UpVector);
					OffsetDirection.Normalize();
					const FVector TargetPosition = AssignTarget->TargetPosition + (OffsetDirection * 100 * i);
					RubyCharacter->SetMovingTarget(TargetPosition);
				}
			}
		}
		else if(!IsCurrentActorsEmpty())
		{
			GetCurrent<ABuilding>(0)->SetSpawnDestination(AssignTarget->TargetPosition);
		}
	}
	else if(RequestAction.IsSameType(NAME_SetMovingTargetActor))
	{
		const TUniquePtr<FSetMovingTargetActor> AssignTarget = MakeUnique<FSetMovingTargetActor>(RequestAction, this);
		
		if (IInteractableInterface* NewTargetInterface = Cast<IInteractableInterface>(AssignTarget->TargetActor))
		{
			for (uint8 i = 0; i < CurrentActors.Num(); i++)
			{
				GetCurrent<ARubyCharacter>(i)->SetMovingTarget(AssignTarget->TargetActor);
			}			
		}
	}
	else if (RequestAction.IsSameType(NAME_Spawn))
	{
		const TUniquePtr<FSpawn> Spawn = MakeUnique<FSpawn>(RequestAction, this);

		if (GetPlayerState<ARubyPlayerState>()->UpdateValueWithType(EResourceType::RT_Wood, Spawn->Price))
		{
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.bNoFail = true;
			ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			UClass* Class = FindObject<UClass>(ANY_PACKAGE, *Spawn->ClassToSpawn);
			AActor* NewActor = GetWorld()->SpawnActor<AActor>(Class, Spawn->SpawnLocation, FRotator::MakeFromEuler(FVector(0,0,90)), ActorSpawnParameters);
			Cast<IInteractableInterface>(NewActor)->Init(TeamId);

			if (ABuilding* TempBuidlingToSpawn = Cast<ABuilding>(NewActor))
			{
				SetBuildingToSpawn(TempBuidlingToSpawn);
				TempBuidlingToSpawn->SetOwner(this);
			}
			else if (ARubyCharacter* NewCharacter = Cast<ARubyCharacter>(NewActor))
			{
				NewCharacter->SetMovingTarget(GetCurrent<ABuilding>(0)->SpawnDestinationSphere->GetComponentLocation());
			}

		}
	}
	
	if (bShouldMulticast)
	{
		Multicast_ActionResult(RequestActionCopy.Get());
	}
}
#pragma endregion 
#pragma region MULTICAST
void ARubyPlayerController::Multicast_ActionResult_Implementation(const FActionRequest& RequestAction)
{	
}
#pragma endregion 
#pragma region CLIENT
void ARubyPlayerController::Client_Init_Implementation(ACameraActor* NewCameraActor)
{
	CameraActor = NewCameraActor;
	SetViewTargetWithBlend(NewCameraActor);
	MainHud = Cast<ARubyHUD>(GetHUD());
	HudGameComponentBP->Init(MainHud);
	//HudGameComponentBP->SleepButton->OnClicked.AddUniqueDynamic(this, &ARubyPlayerController::Server_OnSleepClicked);
	GetPlayerState<ARubyPlayerState>()->Init(HudGameComponentBP);
}
void ARubyPlayerController::Client_ChangeUiVisibility_Implementation(AActor* Actor, const bool bNewIsVisible)
{
	ESlateVisibility NewVisibility = ESlateVisibility::Hidden;
	MainHud->SpawnGridPanel->ClearChildren();
	if(IInteractableInterface* Interface = Cast<IInteractableInterface>(Actor))
	{
		if (ABuilding* Building = Cast<ABuilding>(Actor))
		{
			if (!Building->bIsBuilt) return;
		}

		Interface->GetSelectWidget()->SetHiddenInGame(!bNewIsVisible);

		if (bNewIsVisible)
		{
			if (Actor->IsA(ARubyCharacter::StaticClass()))
			{
				NewVisibility = ESlateVisibility::Visible;
			}
			uint8 Count = 0;
			FVector ActorLocation; 

			if (ABuilding* Building = Cast<ABuilding>(Actor))
			{
				ActorLocation = Building->SpawnPoint->GetComponentLocation();
			}
			else 
			{
				ActorLocation = Actor->GetActorLocation();
			}

			for (const TPair<TSubclassOf<AActor>, int32> Pair : Interface->Execute_GetSpawnableMap(Actor))
			{
				URubyUserWidget* Widget = CreateWidget<URubyUserWidget>(this, RubyUserWidgetClass);
				Widget->Init(Pair.Key, ActorLocation, this, Pair.Value);
				MainHud->SpawnGridPanel->AddChildToGrid(Widget)->SetColumn(Count);
				++Count;
			}
		}
	}
	HudGameComponentBP->BarPanel->SetVisibility(NewVisibility);
}
void ARubyPlayerController::Client_UpdateHudValues_Implementation(const float NewHungerPercent, const float NewTiredPercent, const float NewAgePercent, const EResourceType ResourceType, const float ResourceCount)
{
	HudGameComponentBP->UpdateHudValues(NewHungerPercent, NewTiredPercent, NewAgePercent, ResourceType, ResourceCount);
}

#pragma endregion 
#pragma endregion

void ARubyPlayerController::EmptyCurrentActors()
{
	for (uint8 i = 0; i < CurrentActors.Num(); i++)
	{
		Cast<IInteractableInterface>(CurrentActors[i])->Unselect(this);
		Client_ChangeUiVisibility(CurrentActors[i], false);
	}
	CurrentActors.Empty();
}