// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
UENUM(BlueprintType)
enum class EResourceType : uint8
{
	RT_Wood			UMETA(DisplayName = "Wood"),
	RT_Meat			UMETA(DisplayName = "Meat"),
	RT_Vegetables	UMETA(DisplayName = "Vegetables"),
	RT_Max			UMETA(DisplayName = "Max")
};
//Allow bitwise operation
ENUM_CLASS_FLAGS(EResourceType)

