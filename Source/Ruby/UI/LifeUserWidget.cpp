// Fill out your copyright notice in the Description page of Project Settings.
#include "LifeUserWidget.h"
#include "Components/ProgressBar.h"
void ULifeUserWidget::LifeUpdated(const float NewLifePercent)
{
	if (LifeBar != nullptr)
	{
		LifeBar->SetPercent(NewLifePercent);
	}
}

