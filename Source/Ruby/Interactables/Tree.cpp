// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"
#include "Characters/RubyCharacter.h"


// Sets default values
ATree::ATree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ATree::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATree::Init(const uint8 NewTeamId) 
{
}

bool ATree::Interact(class ARubyCharacter* Interactor)
{
	CurrentLife -= Interactor->Damage;
	return CurrentLife <= 0;
}

