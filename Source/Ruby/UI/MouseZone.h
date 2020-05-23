

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MouseZone.generated.h"

UCLASS()
class RUBY_API AMouseZone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMouseZone();
	void UpdateScale(const FVector NewScale) const;

	FORCEINLINE const TArray<AActor*>& GetOverlappedActors() const { return OverlappedActors; }
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* CollisionMesh = nullptr;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
		TArray<AActor*> OverlappedActors;	
};
