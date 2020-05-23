// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Ruby/Interactables/Buildings/Building.h"
#include "Ruby/Interactables/Characters/RubyCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Ruby/Helpers/Request/ActionRequest.h"
#include "Ruby/Helpers/Request/ActionRequestData.h"
#include "RubyPlayerController.generated.h"


enum class EResourceType : unsigned char;
class ARubyHUD;

UCLASS()
class ARubyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARubyPlayerController();
	
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class URubyUserWidget> RubyUserWidgetClass;
	UPROPERTY(BlueprintReadOnly)
		int32 TeamId { -1 };
	UPROPERTY(BlueprintReadOnly)
		class ACameraActor* CameraActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UWidgetComponent* LifeWidget;
	
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_Init(class ACameraActor* NewCameraActor, int32 NewPlayerId, TSubclassOf<ARubyCharacter> ClassToSpawn);
	UFUNCTION(Server, reliable, WithValidation)
		void Server_RequestAction(const FActionRequest& RequestAction);
	UFUNCTION(NetMulticast, reliable)
		void Multicast_ActionResult(const FActionRequest& RequestAction);
	UFUNCTION(Client, Reliable)
		void Client_ChangeUiVisibility(AActor* Actor, const bool bNewIsVisible);
	UFUNCTION(Client, Reliable)
		void Client_Init(class ACameraActor* NewCameraActor);
	UFUNCTION(Client, Reliable)
		void Client_UpdateHudValues(const float NewHungerPercent, const float NewTiredPercent, const float NewAgePercent, const EResourceType ResourceType, const float ResourceCount);
	
	

	ARubyHUD* MainHud;
	
	FORCEINLINE void SetBuildingToSpawn(ABuilding* NewBuildingToSpawn) { BuildingToSpawn = NewBuildingToSpawn; };

	
	UFUNCTION(BlueprintCallable)
	void Spawn(TSubclassOf<AActor> ClassToSpawn, const FVector NewSpawnLocation, const uint8 Price);
	
	UPROPERTY(BlueprintReadWrite)
		TArray<AActor*> CurrentActors;
private :
	void MoveToMouseCursor();
	void PlayerTick(float DeltaTime) override;
	void SetupInputComponent() override;
	void LeftClick(const bool bStartClick);
	void Zoom(const float ZoomValue);
	void LeftClickedActor(AActor* ClickedActor);
	void UpdateCameraPosition();

	class UHudGameComponent* HudGameComponentBP;

	

	void EmptyCurrentActors();
	
	FORCEINLINE bool IsCurrentActorsEmpty() const { return CurrentActors.Num() == 0; }

	FORCEINLINE bool IsCharacterSelected() const { return !IsCurrentActorsEmpty() && GetCurrent<AActor>(0)->IsA(ARubyCharacter::StaticClass()); }

	template<class T>
	FORCEINLINE T* GetCurrent(const uint8 Index) const { return CurrentActors.Num() > Index ? CastChecked<T>(CurrentActors[Index]) : nullptr; }	
	
	UPROPERTY(Replicated)
		ABuilding* BuildingToSpawn;
	
	

	
	FVector StartClickPosition;

	const uint8 Cam_Speed = 10;

	UPROPERTY()
		class AMouseZone* MouseZone;
};

