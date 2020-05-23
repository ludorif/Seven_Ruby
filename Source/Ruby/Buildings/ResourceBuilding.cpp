// Fill out your copyright notice in the Description page of Project Settings.
#include "ResourceBuilding.h"
#include "UnrealMainClasses/RubyPlayerController.h"
#include "TimerManager.h"
#include "Helpers/ResourceType.h"
#include "Helpers/RubyLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameState.h"
#include "RubyPlayerState.h"

void AResourceBuilding::Init(const uint8 NewTeamId)
{
	ABuilding::Init(NewTeamId);
	FTimerHandle TimerHandle;
	FTimerDelegate TimerCallback;
	float Delay = 0;

	if (AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld()))
	{
		if (ARubyPlayerState* PlayerState = Cast<ARubyPlayerState>(GS->PlayerArray[NewTeamId]))
		{
			EResourceType ResourceType;
			if (WoodProductionDelay > 0)
			{
				Delay = WoodProductionDelay;				
			}
			else if (VegetablesProductionDelay > 0)
			{
				ResourceType = EResourceType::RT_Vegetables;
			}
			else if (MeatProductionDelay > 0)
			{
				ResourceType = EResourceType::RT_Meat;
			}

			TimerCallback.BindLambda([PlayerState, ResourceType]
			{
				PlayerState->UpdateValueWithType(ResourceType);
			});
			
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerCallback, Delay, true);
		}
	}
}

