#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CollectableItem.generated.h"

UCLASS()
class LUDUMDARE56_API ACollectableItem : public AActor
{
    GENERATED_BODY()

public:
    ACollectableItem();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void ApplyEffect(AActor* PlayerActor);

protected:
    UPROPERTY(VisibleAnywhere, Category = "Mesh")
    UStaticMeshComponent* CollectableMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundBase* CollectSound;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
