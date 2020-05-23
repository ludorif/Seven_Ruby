// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../InteractableInterface.h"
#include "Ruby/Helpers/ResourceType.h"
#include "RubyCharacter.generated.h"
USTRUCT()
struct FStatePercent {
	GENERATED_BODY()
public:
	FStatePercent() : Percent(1.0f) {};
	void UpdatePercent(const float ToAdd, uint8& CurrentLife)
	{
		if (Percent > 0.0f)
		{
			Percent += ToAdd;
			Percent = FMath::Min(Percent, 1.0f);
		}
		else
		{
			CurrentLife += ToAdd;
		}
	}
	FORCEINLINE float GetPercent() { return Percent; };
private:
	float Percent;
};
UENUM(BlueprintType)
enum class EActionState : uint8
{
	AS_None				UMETA(DisplayName = "None"),
	AS_Idle				UMETA(DisplayName = "Idle"),
	AS_Walking			UMETA(DisplayName = "Walking"),
	AS_Interacting			UMETA(DisplayName = "Interacting"),
};
UCLASS(Blueprintable)
class ARubyCharacter : public ACharacter, public IInteractableInterface
{
	GENERATED_BODY()
public:
	ARubyCharacter();
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	void Init(const uint8 NewTeamId) override;
	UFUNCTION(NetMulticast, Reliable)
		virtual void Multicast_Init(const uint8 NewTeamId);

	void SetMovingTarget(const FVector& NewTargetLocation);
	UFUNCTION(BlueprintCallable)
		void SetMovingTarget(AActor* NewTarget);

	FORCEINLINE void ChangeSleepState() { bIsSleeping *= -1;  };	
	bool Eat(const EResourceType ResourceType, const float EatCount);
	UPROPERTY(BlueprintReadWrite) // Set in bp collision livreur
		ARubyCharacter* CloseLivreur;

	const uint8 Damage = 1;//todo configurable

	UWidgetComponent* GetSelectWidget() override { return SelectWidget; }

protected:
	UFUNCTION(BlueprintImplementableEvent)
		void ChangeActionState(const EActionState& NewState);

	UFUNCTION(BlueprintImplementableEvent)
		void OnInteractStoppingBP(UClass* OldTarget);

	UPROPERTY(BlueprintReadWrite)
		EActionState CurrentState { EActionState::AS_None};

	UFUNCTION(BlueprintCallable)
		uint8 GetTeamId() { return TeamId;}

private:
	//todo clean

	int8 bIsSleeping {-1}; // -1 or 1;
	FStatePercent HungerPercent;
	FStatePercent TiredPercent;
	FStatePercent AgePercent;

	

	UPROPERTY()
		AActor* Target;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* LifeWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* SelectWidget;

	bool Interact(ARubyCharacter* Interactor) override;
	void StopMovingCustom();
	UPROPERTY(EditDefaultsOnly)
		uint8 TotalLife {100};
	UPROPERTY(EditDefaultsOnly)
		float HungerSpeed {0.02f};
	UPROPERTY(EditDefaultsOnly)
		float TiredSpeed{ 0.03f };
	UPROPERTY(EditDefaultsOnly)
		float AgingSpeed{ 0.01f };
	UPROPERTY(EditDefaultsOnly)
		EResourceType ResourceToEat;
	UPROPERTY(EditDefaultsOnly)
		float ResourceCount;

	FVector TargetLocation;
	
	UPROPERTY(ReplicatedUsing = OnLifePercentChanged)
		float LifePercent;

	UFUNCTION()
		void OnLifePercentChanged();

	
	float LastVelocity;
};
