// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LifeUserWidget.generated.h"
/**
 * 
 */
UCLASS()
class RUBY_API ULifeUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void LifeUpdated(const float NewLifePercent);
	UPROPERTY(BlueprintReadWrite)
	class UProgressBar* LifeBar;
};
