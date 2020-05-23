// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "Components/ActorComponent.h"
#include "Helpers/ResourceType.h"
#include "HudGameComponent.generated.h"
class UProgressBar;
class UTextBlock;
/// <summary>
/// Component shared by both human and ghost to prevent code duplication
/// </summary>
UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UHudGameComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UFUNCTION(BlueprintImplementableEvent)
		void Init(class ARubyHUD* MainHud);
	UFUNCTION(BlueprintNativeEvent)
		void UpdateHudValues(const float NewHungerPercent, const float NewTiredPercent, const float NewAgePercent, const EResourceType ResourceType, const float ResourceCount);
	UFUNCTION()
		void UpdateResourceValues(const TMap<EResourceType, uint16>& ResourcesMap) const;
	static void UpdateResourceValue(UTextBlock* TextBlock, uint16 ResourceCount);

	UPROPERTY(BlueprintReadWrite)
		UProgressBar* HungerBar;
	UPROPERTY(BlueprintReadWrite)
		UProgressBar* TiredBar;
	UPROPERTY(BlueprintReadWrite)
		UProgressBar* AgeBar;

	UPROPERTY(BlueprintReadWrite)
		UTextBlock* WoodText;
	UPROPERTY(BlueprintReadWrite)
		UTextBlock* MeatText;
	UPROPERTY(BlueprintReadWrite)
		UTextBlock* VegetableText;
	
	UPROPERTY(BlueprintReadWrite)
		class UCanvasPanel* BarPanel;
	UPROPERTY(BlueprintReadWrite)
		class UButton* SleepButton;
};
