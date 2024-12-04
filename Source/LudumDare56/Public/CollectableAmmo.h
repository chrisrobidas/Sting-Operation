#pragma once

#include "CoreMinimal.h"
#include "CollectableItem.h"
#include "CollectableAmmo.generated.h"

UCLASS()
class LUDUMDARE56_API ACollectableAmmo : public ACollectableItem
{
    GENERATED_BODY()

public:
    ACollectableAmmo();

protected:
    virtual void ApplyEffect(AActor* PlayerActor) override;

private:
    UPROPERTY(EditAnywhere, Category = "Ammo")
    int32 AmmoAmount = 3;
};
