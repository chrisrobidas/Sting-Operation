#include "BossAICharacter.h"
#include "FirstPersonCharacter.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Components/ProgressBar.h>
#include <Components/TextBlock.h>

ABossAICharacter::ABossAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Health = 100.0f;
	MovementSpeed = DefaultMovementSpeed;
	StoppingDistance = 600.0f;
	DamageAmount = 35.f;
	DamageCooldown = 2.0f;
	LastDamageTime = 0.0f;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetAutoActivate(true);
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->SetRelativeLocation(FVector::ZeroVector);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
}

void ABossAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (BossHealthWidgetClass != nullptr)
	{
		BossHealthWidget = CreateWidget<UUserWidget>(PlayerController, BossHealthWidgetClass);
		BossHealthWidget->AddToViewport();
	}
}

void ABossAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerCharacter)
	{
		MoveTowardsPlayer();
	}
}

void ABossAICharacter::MoveTowardsPlayer()
{
	if (!PlayerCharacter) return;

	// Get the distance between the enemy and the player
	float DistanceToPlayerPosition = FVector::Dist(PlayerCharacter->GetActorLocation(), GetActorLocation());

	// Stop moving if the enemy is close enough to the player
	if (DistanceToPlayerPosition > StoppingDistance)
	{
		// Get the direction to the player
		FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
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

		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - LastDamageTime >= DamageCooldown)
		{
			DealDamage();
			LastDamageTime = CurrentTime;
		}
	}
}

void ABossAICharacter::DealDamage()
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

void ABossAICharacter::Hurt(float Damage)
{
	Health -= Damage;

	if (Health <= 0) {
		Health = 0;
		EndGame();
	}

	if (BossHealthWidget)
	{
		UTextBlock* HealthText = Cast<UTextBlock>(BossHealthWidget->GetWidgetFromName("HealthText"));
		if (HealthText)
		{
			// Update the text to show current health
			FString HealthString = FString::Printf(TEXT("Health: %d"), FMath::RoundToInt(Health));
			HealthText->SetText(FText::FromString(HealthString));
		}

		UProgressBar* HealthBar = Cast<UProgressBar>(BossHealthWidget->GetWidgetFromName("HealthBar"));
		if (HealthBar)
		{
			float HealthPercent = Health / 100.0f;
			HealthBar->SetPercent(HealthPercent);
		}
	}
}

void ABossAICharacter::EndGame()
{
	AudioComponent->Stop();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (VictoryWidgetClass != nullptr)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PlayerController, VictoryWidgetClass);
		GameOverWidget->AddToViewport();
		PlayerController->bShowMouseCursor = true;
	}
}
