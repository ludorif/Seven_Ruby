#include "RubyPlayerState.h"
#include "Characters/Components/HudGameComponent.h"
#include "UnrealNetwork.h"
#include "Helpers/ResourceType.h"

ARubyPlayerState::ARubyPlayerState()
{
	const uint8 BaseValue = 100;
	ResourcesMap = TMap<EResourceType, uint16>();

	for (uint8 i = static_cast<uint8>(EResourceType::RT_None) + 1;
		i < static_cast<uint8>(EResourceType::RT_Max);
		++i)
	{
		ResourcesMap.Add(static_cast<EResourceType>(i), BaseValue);
	}
}

void ARubyPlayerState::Init(UHudGameComponent* HudGameComponentBP)
{
	this->HudGameComponentBP = HudGameComponentBP;
}

void ARubyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARubyPlayerState, ResourcesMap);
}

bool ARubyPlayerState::UpdateValueWithType(const EResourceType ResourceType, const int16 ValueToAdd)
{
	//ResourcesMap not replicated ??????????????????????
	if ((static_cast<int16>(ResourcesMap[ResourceType]) + ValueToAdd) >= 0)
	{
		ResourcesMap[ResourceType] += ValueToAdd;
		return true;
	}
	return false;	
}


void ARubyPlayerState::OnRep_Resources() const
{
	HudGameComponentBP->UpdateResourceValues(ResourcesMap);
}