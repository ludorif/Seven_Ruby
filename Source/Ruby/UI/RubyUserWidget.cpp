// Fill out your copyright notice in the Description page of Project Settings.
#include "RubyUserWidget.h"
#include "Components/Button.h"
#include "Ruby/UnrealMainClasses/RubyPlayerController.h"

void URubyUserWidget::Init_Implementation(TSubclassOf<AActor> NewClassToSpawn, const FVector& NewSpawnLocation, ARubyPlayerController* NewRubyPlayerController, const uint8 NewPrice)
{
	ClassToSpawn = NewClassToSpawn;
	SpawnLocation = NewSpawnLocation;
	RubyPlayerController = NewRubyPlayerController;
	Price = NewPrice;
	SpawnButton->OnClicked.AddUniqueDynamic(this, &URubyUserWidget::Spawn);
}
void URubyUserWidget::Spawn()
{
	RubyPlayerController->Spawn(ClassToSpawn, SpawnLocation, Price);
}

