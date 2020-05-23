// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/Widget.h"
#include "../InteractableInterface.h"
#include "Building.generated.h"
UCLASS()
class RUBY_API ABuilding : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ABuilding();
	void ShowBuildingUI(class ARubyPlayerController* PlayerController, class UPanelWidget* ParentWidget);
	
	void Select(ARubyPlayerController* PlayerController) override;
	void Unselect(ARubyPlayerController* PlayerController) override;
	void Init(const uint8 NewTeamId) override;
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_Init(const uint8 NewTeamId);
	UFUNCTION(NetMulticast, Reliable)
		void Multicast_ChangeSpawnDestinationVisibility(const bool NewVisibility);
	UFUNCTION(Server, Reliable, WithValidation)
		void Server_SetPosition(const FVector_NetQuantize& NewLocation);
	bool Interact(ARubyCharacter* Interactor) override;

	UPROPERTY(EditDefaultsOnly)
		uint8 TotalLife {100};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USceneComponent* SpawnPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* SpawnDestinationSphere;
	
	void SetSpawnDestination(const FVector& NewSpawnDestination) const;

	UPROPERTY(Replicated)
		bool bIsBuilt;

	UFUNCTION(BlueprintImplementableEvent)
		void BuiltBP();

	 UWidgetComponent* GetSelectWidget() override { return SelectWidget; }
	
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UChildActorComponent* ChildActorComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* LifeWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* SelectWidget;

	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_LifeUpdated(const float LifePercent);

};
