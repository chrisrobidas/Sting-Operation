#include "FirstPersonWeaponComponent.h"
#include "FirstPersonCharacter.h"
#include "BossAICharacter.h"
#include "FirstPersonProjectile.h"
#include "FirstPersonDart.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnemyAICharacter.h"

UFirstPersonWeaponComponent::UFirstPersonWeaponComponent()
{
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
	PrimaryComponentTick.bCanEverTick = true;
}

void UFirstPersonWeaponComponent::PrimaryFire()
{
	if (Character == nullptr || Character->GetController() == nullptr || GetWorld() == nullptr || Character->Health == 0 || Character->IsPaused)
	{
		return;
	}

	if (DartAmmunitionCount <= 0)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		DartAmmunitionCount--;
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector start = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
		const FVector forward = PlayerController->PlayerCameraManager->GetActorForwardVector();
		const FVector end = start + (forward * PrimaryFireRayMaxDistance);


		if (UWorld* const World = GetWorld())
		{
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<AFirstPersonDart>(DartClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

		TArray <FHitResult> hits;
		GetWorld()->LineTraceMultiByObjectType(hits, start, end, FCollisionObjectQueryParams::AllObjects);
		for (const FHitResult& hit : hits)
		{
			if (AEnemyAICharacter* enemy = Cast<AEnemyAICharacter>(hit.GetActor()))
			{
				enemy->Death();
			}
			else if (ABossAICharacter* boss = Cast<ABossAICharacter>(hit.GetActor()))
			{
				boss->Hurt(1.0f);
			}
		}
	}

	if (PrimaryFireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PrimaryFireSound, Character->GetActorLocation(), 0.2f);
	}

	if (FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UFirstPersonWeaponComponent::SecondaryFire()
{
	if (Character == nullptr || Character->GetController() == nullptr || Character->Health == 0 || Character->IsPaused)
	{
		return;
	}

	bool canFire = SecondaryFireCharge >= SecondaryFireChargeConsumption;

	if (ProjectileClass == nullptr || !canFire)
	{
		return;
	}

	if (UWorld* const World = GetWorld())
	{
		SecondaryFireCharge -= SecondaryFireChargeConsumption;
		APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// Spawn the projectile at the muzzle
		World->SpawnActor<AFirstPersonProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}

	if (SecondaryFireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SecondaryFireSound, Character->GetActorLocation(), 0.2f);
	}

	if (FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UFirstPersonWeaponComponent::ReceiveDartAmmunition(int ammoCount)
{
	if (ammoCount > 0)
	{
		DartAmmunitionCount += ammoCount;
	}
}

bool UFirstPersonWeaponComponent::AttachWeapon(AFirstPersonCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UFirstPersonWeaponComponent>())
	{
		return false;
	}

	UGameplayStatics::PlaySoundAtLocation(this, CollectSound, Character->GetActorLocation(), 1.0f);

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// Add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			EnhancedInputComponent->BindAction(PrimaryFireAction, ETriggerEvent::Triggered, this, &UFirstPersonWeaponComponent::PrimaryFire);
			EnhancedInputComponent->BindAction(SecondaryFireAction, ETriggerEvent::Triggered, this, &UFirstPersonWeaponComponent::SecondaryFire);
		}
	}

	return true;
}

void UFirstPersonWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UFirstPersonWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SecondaryFireCharge = FMath::Min<float>(SecondaryFireMaxCharge, SecondaryFireCharge + DeltaTime * 10.0f);

	if (Character != nullptr)
	{
		return;
	}

	// Rotation (rotate around Z-axis)
	FRotator NewRotation = GetRelativeRotation();
	NewRotation.Yaw += DeltaTime * 50.0f;  // Adjust 50.0f to control the speed of rotation
	SetRelativeRotation(NewRotation);

	// Up and down movement (sinusoidal motion)
	FVector NewLocation = GetRelativeLocation();
	float DeltaHeight = FMath::Sin(GetWorld()->GetTimeSeconds() * 2.0f); // Oscillate with a period of roughly 3.14 seconds
	NewLocation.Z += DeltaHeight * 20.0f * DeltaTime;  // Adjust 20.0f to control the amplitude of the movement
	SetRelativeLocation(NewLocation);
}
