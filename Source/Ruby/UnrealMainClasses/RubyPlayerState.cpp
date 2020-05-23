#include "RubyPlayerState.h"
#include "Ruby/Interactables/Characters/Components/HudGameComponent.h"
#include "Net/UnrealNetwork.h"
#include "Ruby/Helpers/ResourceType.h"

ARubyPlayerState::ARubyPlayerState()
{
	ResourcesArr = TArray<uint16>();	
	const uint8 BaseValue = 100;
	for (uint8 i = 0; i < static_cast<uint8>(EResourceType::RT_Max); ++i)
	{
		ResourcesArr.Add(BaseValue);
	}
}

void ARubyPlayerState::Init(UHudGameComponent* NewHudGameComponentBP)
{
	if (NewHudGameComponentBP != nullptr)
	{
		HudGameComponentBP = NewHudGameComponentBP;
		HudGameComponentBP->UpdateResourceValues(ResourcesArr);
	}
}

void ARubyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARubyPlayerState, ResourcesArr, COND_OwnerOnly);
}

bool ARubyPlayerState::UpdateValueWithType(const EResourceType ResourceType, const int32 ValueToAdd)
{
	const uint8 ResourceTypeIdx = static_cast<uint8>(ResourceType);
	if ((static_cast<int16>(ResourcesArr[ResourceTypeIdx]) + ValueToAdd) >= 0)
	{
		ResourcesArr[ResourceTypeIdx] += ValueToAdd;
		return true;
	}
	return false;	
}


void ARubyPlayerState::OnRep_Resources() const
{
	HudGameComponentBP->UpdateResourceValues(ResourcesArr);
}