// Fill out your copyright notice in the Description page of Project Settings.
#include "HudGameComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHudGameComponent::UpdateHudValues_Implementation(const float NewHungerPercent, const float NewTiredPercent, const float NewAgePercent, const EResourceType ResourceType, const float ResourceCount)
{
	if(HungerBar != nullptr)
	{
		HungerBar->SetPercent(NewHungerPercent);
		TiredBar->SetPercent(NewTiredPercent);
		AgeBar->SetPercent(NewAgePercent);
	}
}

void UHudGameComponent::UpdateResourceValues(const TArray<uint16>& ResourcesArr) const
{
	UpdateResourceValue(MeatText, EResourceType::RT_Meat, ResourcesArr);
	UpdateResourceValue(VegetableText, EResourceType::RT_Vegetables, ResourcesArr);
	UpdateResourceValue(WoodText, EResourceType::RT_Wood, ResourcesArr);
}

void UHudGameComponent::UpdateResourceValue(UTextBlock* TextBlock, const EResourceType ResourceType, const TArray<uint16>& ResourcesArr)
{
	const uint8 ResourceTypeIdx = static_cast<uint8>(ResourceType);
	const FString ResourceStr = FString::FromInt(ResourcesArr[ResourceTypeIdx]);
	TextBlock->SetText(FText::FromString(ResourceStr));
}
