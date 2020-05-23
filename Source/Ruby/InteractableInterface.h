// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Interface.h"
#include "Components/WidgetComponent.h"
#include "ConstructorHelpers.h"
#include "InteractableInterface.generated.h"
class UWidgetComponent;
class ARubyPlayerController;
/* must have BlueprintType as a specifier to have this interface exposed to blueprints
with this line you can easily add this interface to any blueprint class */
UINTERFACE(BlueprintType)
class RUBY_API UInteractableInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};
class RUBY_API IInteractableInterface
{
	GENERATED_IINTERFACE_BODY()
public:
	class UWidgetComponent* LifeWidget;
	class UWidgetComponent* SelectWidget;
	ARubyPlayerController* RubyPlayerController;
	virtual void Select(ARubyPlayerController* PlayerController) 
	{
		RubyPlayerController = PlayerController;
	}
	
	virtual void Unselect(ARubyPlayerController* PlayerController) {}
	virtual void Init(const uint8 NewTeamId) = 0;
	virtual bool Interact(class ARubyCharacter* Interactor) = 0;
	
	void InitWidget(UWidgetComponent* WidgetComponent, const TSubclassOf<UUserWidget>& InWidgetClass, const FVector2D& Size)
	{
		WidgetComponent->SetWorldRotation(FRotator(90.f, 0.0f, 0.f));
		WidgetComponent->SetWorldLocation(FVector(0, 0, 150.0f));
		WidgetComponent->SetWidgetClass(InWidgetClass);
		WidgetComponent->SetDrawSize(Size);
	};
	void InitSelectWidget(UWidgetComponent* SelectWidgetComponent)
	{
		static ConstructorHelpers::FClassFinder<UUserWidget> SelectWidgetClass(TEXT("WidgetBlueprint'/Game/Blueprint/UI/SelectWidgetBP.SelectWidgetBP_C'"));
		InitWidget(SelectWidgetComponent, SelectWidgetClass.Class, FVector2D(150.0f, 150.0f));
		SelectWidgetComponent->SetHiddenInGame(true);
	};
	void InitLifeWidget(UWidgetComponent* LifeWidgetComponent)
	{
		static ConstructorHelpers::FClassFinder<UUserWidget> LifeWidgetClass(TEXT("WidgetBlueprint'/Game/Blueprint/UI/LifeWidgetBP.LifeWidgetBP_C'"));
		InitWidget(LifeWidgetComponent, LifeWidgetClass.Class, FVector2D(100.0f, 10.0f));
	};
	uint8 CurrentLife {0};
	int32 TeamId;
	UFUNCTION(BlueprintImplementableEvent)
		TMap<TSubclassOf<AActor>, int32> GetSpawnableMap();


	FORCEINLINE bool IsSameTeam(const uint8 OtherTeamId) const { return TeamId == OtherTeamId; }
};

