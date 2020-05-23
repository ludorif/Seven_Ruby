

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
	
	void Init(UHudGameComponent* NewHudGameComponentBP);
	
	UFUNCTION(BlueprintCallable)
	//Return if there is enough resource
	bool UpdateValueWithType(const EResourceType ResourceType, const int32 ValueToAdd = 1);
	
protected:
	UPROPERTY(BlueprintReadOnly)
		UHudGameComponent* HudGameComponentBP = { nullptr };

private:

	UPROPERTY(ReplicatedUsing = OnRep_Resources)
		TArray<uint16> ResourcesArr;
	
	UFUNCTION()
		void OnRep_Resources() const;

	
};
