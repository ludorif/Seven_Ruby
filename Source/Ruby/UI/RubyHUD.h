// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RubyHUD.generated.h"
/**
 * 
 */
UCLASS()
class RUBY_API ARubyHUD : public AHUD
{
	GENERATED_BODY()
public:
	ARubyHUD();
	UPROPERTY(BlueprintReadWrite)
	class UGridPanel* SpawnGridPanel;
};
