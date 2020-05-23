

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RubyPlayerState.generated.h"

class UHudGameComponent;
enum class EResourceType : unsigned char;

/**
 * 
 */
UCLASS()
class RUBY_API ARubyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ARubyPlayerState();
	
	void Init(UHudGameComponent* HudGameComponentBP);
	
	//Return if there is enough resource
	bool UpdateValueWithType(const EResourceType ResourceType, const int16 ValueToAdd = 1);
	
private:

	UPROPERTY(ReplicatedUsing = OnRep_Resources)
		TMap<EResourceType, uint16> ResourcesMap;
	
	UFUNCTION()
		void OnRep_Resources() const;

	UHudGameComponent* HudGameComponentBP = {nullptr};
};
