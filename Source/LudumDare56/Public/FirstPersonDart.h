#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FirstPersonDart.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS(config = Game)
class AFirstPersonDart : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* meshComp;

public:
	AFirstPersonDart();
};
