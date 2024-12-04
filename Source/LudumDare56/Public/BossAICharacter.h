#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossAICharacter.generated.h"

UCLASS()
class LUDUMDARE56_API ABossAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABossAICharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioComponent;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DefaultMovementSpeed = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float StoppingDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DamageCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DealDamageAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> BossHealthWidgetClass;

	UPROPERTY()
	UUserWidget* BossHealthWidget;

	float Health;

	void Hurt(float Damage);

private:
	ACharacter* PlayerCharacter;

	FTimerHandle PlayerPositionUpdateTimerHandle;

	void MoveTowardsPlayer();
	void DealDamage();
	void EndGame();

	FTimerHandle DestroyTimerHandle;
	float LastDamageTime;
};
