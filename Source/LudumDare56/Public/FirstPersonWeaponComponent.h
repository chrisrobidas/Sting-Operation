#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "FirstPersonWeaponComponent.generated.h"

class AFirstPersonCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDUMDARE56_API UFirstPersonWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFirstPersonProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFirstPersonDart> DartClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* PrimaryFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* SecondaryFireSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float SecondaryFireCharge = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float SecondaryFireMaxCharge = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float SecondaryFireChargeConsumption = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float PrimaryFireRayMaxDistance = 100000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	int DartAmmunitionCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	float PrimaryFireSoundVolume = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	float SecondaryFireSoundVolume = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PrimaryFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SecondaryFireAction;

	UFirstPersonWeaponComponent();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(AFirstPersonCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PrimaryFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SecondaryFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReceiveDartAmmunition(int ammoCount);

protected:
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* CollectSound;

private:
	/** The Character holding this weapon*/
	AFirstPersonCharacter* Character;
};
