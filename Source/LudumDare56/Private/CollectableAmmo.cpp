#include "CollectableAmmo.h"
#include "FirstPersonCharacter.h"

ACollectableAmmo::ACollectableAmmo()
{
}

void ACollectableAmmo::ApplyEffect(AActor* PlayerActor)
{
    AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(PlayerActor);
    if (Player)
    {
        if (Player->WeaponComponent != nullptr) {
            Player->WeaponComponent->ReceiveDartAmmunition(AmmoAmount);
        }
    }
}
