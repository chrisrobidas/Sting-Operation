#pragma once

#include "CoreMinimal.h"
#include "CollectableItem.h"
#include "CollectableHealth.generated.h"

UCLASS()
class LUDUMDARE56_API ACollectableHealth : public ACollectableItem
{
    GENERATED_BODY()

public:
    ACollectableHealth();

protected:
    virtual void ApplyEffect(AActor* PlayerActor) override;

private:
    UPROPERTY(EditAnywhere, Category = "Health")
    float HealthAmount = 10.0f;
};
