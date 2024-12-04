#pragma once

#include "CollectableItem.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyAICharacter.generated.h"

UCLASS()
class LUDUMDARE56_API AEnemyAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyAICharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioComponent;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool IsFrozen = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DefaultMovementSpeed = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StoppingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DamageCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float UpdatePlayerPositionTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	int HoneyState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float HoneyStateDuration;

	UPROPERTY(EditAnywhere, Category = "Collectables")
	TSubclassOf<ACollectableItem> HealthCollectableClass;

	UPROPERTY(EditAnywhere, Category = "Collectables")
	TSubclassOf<ACollectableItem> AmmoCollectableClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DealDamageAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* HitByHoneySound;

	void Death();
	void HitByHoney();

private:
	ACharacter* PlayerCharacter;
	FVector LastKnownPlayerPosition;

	FTimerHandle PlayerPositionUpdateTimerHandle;

	UFUNCTION()
	void UpdatePlayerPosition();

	void MoveTowardsPlayer();
	void DealDamage();
	void DelayedDestroy();

	FTimerHandle DestroyTimerHandle;
	float LastDamageTime;
	bool IsDead;
};
