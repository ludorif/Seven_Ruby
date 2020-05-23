#pragma once
#include "ActionRequestData.h"

/// How to create new RequestAction : 
///		* DECLARE_REQUEST_ACTION_POINTER : only takes pointer on any instance replicated (custom or not)
///		* DECLARE_REQUEST_ACTION : only takes references of any type (custom or not)
/// Declare your new request action as : 
///	DECLARE_REQUEST_ACTION...(Name, Type1, VariableName1, Type2, VariableName2....);

DECLARE_REQUEST_ACTION(Sleep)
DECLARE_REQUEST_ACTION_ONE_PARAM(SetMovingTargetPosition, FVector, TargetPosition)
DECLARE_REQUEST_ACTION_THREE_PARAMS(Spawn, FString, ClassToSpawn, FVector, SpawnLocation, int32, Price)

DECLARE_REQUEST_ACTION_POINTER_ONE_PARAM(SetMovingTargetActor, AActor*, TargetActor)
DECLARE_REQUEST_ACTION_POINTER_ONE_PARAM(LeftClickedActor, AActor*, ClickedActor)



