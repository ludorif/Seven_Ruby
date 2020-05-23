// Fill out your copyright notice in the Description page of Project Settings.
#include "HudGameComponent.h"
#include "ProgressBar.h"
#include "TextBlock.h"

void UHudGameComponent::UpdateHudValues_Implementation(const float NewHungerPercent, const float NewTiredPercent, const float NewAgePercent, const EResourceType ResourceType, const float ResourceCount)
{
	if(HungerBar != nullptr)
	{
		HungerBar->SetPercent(NewHungerPercent);
		TiredBar->SetPercent(NewTiredPercent);
		AgeBar->SetPercent(NewAgePercent);
	}
}

void UHudGameComponent::UpdateResourceValues(const TMap<EResourceType, uint16>& ResourcesMap) const
{
	UpdateResourceValue(MeatText, ResourcesMap[EResourceType::RT_Meat]);
	UpdateResourceValue(VegetableText, ResourcesMap[EResourceType::RT_Vegetables]);
	UpdateResourceValue(WoodText, ResourcesMap[EResourceType::RT_Wood]);
}

void UHudGameComponent::UpdateResourceValue(UTextBlock* TextBlock, const uint16 ResourceCount)
{
	const FString ResourceStr = FString::FromInt(ResourceCount);
	TextBlock->SetText(FText::FromString(ResourceStr));
}
