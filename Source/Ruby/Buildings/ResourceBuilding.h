// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Buildings/Building.h"
#include "ResourceBuilding.generated.h"
/**
 * 
 */
UCLASS()
class RUBY_API AResourceBuilding : public ABuilding
{
	GENERATED_BODY()
	void Init(const uint8 NewTeamId) override;
	UPROPERTY(EditDefaultsOnly)
		float WoodProductionDelay {0};
	UPROPERTY(EditDefaultsOnly)
		float MeatProductionDelay {0};
	UPROPERTY(EditDefaultsOnly)
		float VegetablesProductionDelay {0};
};
