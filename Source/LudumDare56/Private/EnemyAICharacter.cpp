#include "EnemyAICharacter.h"
#include "FirstPersonCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>

AEnemyAICharacter::AEnemyAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	MovementSpeed = DefaultMovementSpeed;
	StoppingDistance = 100.0f;
	DamageAmount = 10.f;
	DamageCooldown = 2.0f;
	UpdatePlayerPositionTime = 2.0f;
	LastDamageTime = 0.0f;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetAutoActivate(true);
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->SetRelativeLocation(FVector::ZeroVector);

	// Make values a bit organic
	StoppingDistance += FMath::RandRange(0, 10);
	UpdatePlayerPositionTime += FMath::RandRange(0, 4);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
}

void AEnemyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	UpdatePlayerPosition();
	GetWorld()->GetTimerManager().SetTimer(PlayerPositionUpdateTimerHandle, this, &AEnemyAICharacter::UpdatePlayerPosition, UpdatePlayerPositionTime, true);
}

void AEnemyAICharacter::Tick(float DeltaTime)
{
	if (IsDead) return;

	Super::Tick(DeltaTime);

	if (HoneyStateDuration - DeltaTime > 0.0f)
	{
		HoneyStateDuration -= DeltaTime;
	}
	else
	{
		HoneyStateDuration = 0.0;
		HoneyState = 0;
		IsFrozen = false;
		MovementSpeed = DefaultMovementSpeed;
	}

	if (PlayerCharacter && !IsFrozen)
	{
		MoveTowardsPlayer();
	}
}

void AEnemyAICharacter::UpdatePlayerPosition()
{
	if (!PlayerCharacter || IsDead) return;

	LastKnownPlayerPosition = PlayerCharacter->GetActorLocation();
}

void AEnemyAICharacter::MoveTowardsPlayer()
{
	if (!PlayerCharacter || IsDead) return;

	// Get the distance between the enemy and the player
	float DistanceToLastKnownPlayerPosition = FVector::Dist(LastKnownPlayerPosition, GetActorLocation());

	// Stop moving if the enemy is close enough to the player
	if (DistanceToLastKnownPlayerPosition > StoppingDistance)
	{
		// Get the direction to the player
		FVector Direction = LastKnownPlayerPosition - GetActorLocation();
		Direction.Z = 0.0f; // We don't want to change the pitch, only the yaw (rotate on the horizontal axis)
		Direction.Normalize();  // Normalize the direction to get unit vector

		// Add movement in the direction of the player
		AddMovementInput(Direction, MovementSpeed * GetWorld()->GetDeltaSeconds());

		// Rotate to face the player
		FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
		LookAtRotation.Pitch = 0.0f;  // We only want to rotate in the yaw (ignore pitch and roll)
		LookAtRotation.Roll = 0.0f;

		// Smoothly interpolate to face the player
		FRotator CurrentRotation = GetActorRotation();
		SetActorRotation(FMath::RInterpTo(CurrentRotation, LookAtRotation, GetWorld()->GetDeltaSeconds(), 5.0f));
	}
	else 
	{
		// Stop any movement and prevent further rotation when the enemy is close enough
		GetCharacterMovement()->StopMovementImmediately();
	}

	float DistanceToPlayerPosition = FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation());

	if (DistanceToPlayerPosition <= StoppingDistance)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - LastDamageTime >= DamageCooldown)
		{
			DealDamage();
			LastDamageTime = CurrentTime;
		}
	}
}

void AEnemyAICharacter::DealDamage()
{
	if (PlayerCharacter)
	{
		if (DealDamageAnimation != nullptr)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(DealDamageAnimation, 1.0f);
			}
		}

		Cast<AFirstPersonCharacter>(PlayerCharacter)->Hurt(DamageAmount);
	}
}

void AEnemyAICharacter::Death()
{
	if (IsDead) return;

	bool bShouldDropHealth = FMath::RandBool();
	TSubclassOf<ACollectableItem> CollectableClass = bShouldDropHealth ? HealthCollectableClass : AmmoCollectableClass;

	if (CollectableClass)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		GetWorld()->SpawnActor<ACollectableItem>(CollectableClass, SpawnLocation, SpawnRotation);
	}

	if (DeathAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(DeathAnimation, 1.0f);
		}
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	IsDead = true;
	AudioComponent->Stop();
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), 1.0f);

	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AEnemyAICharacter::DelayedDestroy, 5.0f, false);
}

void AEnemyAICharacter::DelayedDestroy()
{
	Destroy();
}

void AEnemyAICharacter::HitByHoney()
{
	if (IsDead) return;

	++HoneyState;

	UGameplayStatics::PlaySoundAtLocation(this, HitByHoneySound, GetActorLocation(), 1.0f);

	switch (HoneyState)
	{
		case 0:
			break;
		case 1:
			MovementSpeed = DefaultMovementSpeed * 0.75f;
			HoneyStateDuration = 2.0f;
			break;
		case 2:
			MovementSpeed = DefaultMovementSpeed * 0.5f;
			HoneyStateDuration = 2.0f;
			break;
		default:
			IsFrozen = true;
			HoneyStateDuration = 4.0f;
			MovementSpeed = 0.0f;
	}
}
