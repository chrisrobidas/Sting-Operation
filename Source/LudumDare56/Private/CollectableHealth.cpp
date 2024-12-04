#include "CollectableHealth.h"
#include "FirstPersonCharacter.h"

ACollectableHealth::ACollectableHealth()
{
}

void ACollectableHealth::ApplyEffect(AActor* PlayerActor)
{
    AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(PlayerActor);
    if (Player)
    {
        Player->Heal(HealthAmount);
    }
}
