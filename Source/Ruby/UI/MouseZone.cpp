#include "MouseZone.h"
#include "Ruby/Interactables/Buildings/Building.h"
#include "Ruby/Interactables/Characters/RubyCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AMouseZone::AMouseZone()
{
	CollisionMesh = CreateDefaultSubobject<UStaticMeshComponent>("CollisionMesh");
	SetRootComponent(CollisionMesh);
	CollisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	CollisionMesh->SetStaticMesh(SphereMeshAsset.Object);
	CollisionMesh->OnComponentBeginOverlap.AddUniqueDynamic(this, &AMouseZone::OnBeginOverlap);

	OverlappedActors = TArray<AActor*>();
	CollisionMesh->SetWorldScale3D(FVector::ZeroVector);
}

void AMouseZone::UpdateScale(const FVector NewScale) const
{
    CollisionMesh->SetWorldScale3D(NewScale);
}

void AMouseZone::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ARubyCharacter* Character = Cast<ARubyCharacter>(OtherActor))
	{
		OverlappedActors.Emplace(Character);
	}
	else if (ABuilding* Building = Cast<ABuilding>(OtherActor))
	{
		OverlappedActors.Empty();
		OverlappedActors.Emplace(Building);
	}
}




