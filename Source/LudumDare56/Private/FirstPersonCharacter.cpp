#include "FirstPersonCharacter.h"
#include "FirstPersonProjectile.h"
#include "Blueprint/UserWidget.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AFirstPersonCharacter::AFirstPersonCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Health = 100.0f;
	AimSensitivity = 1.0f;

	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetAutoActivate(true);
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->SetRelativeLocation(FVector::ZeroVector);
	AudioComponent->SetPaused(true);

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void AFirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AFirstPersonCharacter::Tick(float DeltaTime)
{
	if (GetVelocity().Size() > KINDA_SMALL_NUMBER && GetCharacterMovement()->IsMovingOnGround())
	{
		AudioComponent->SetPaused(false);
	}
	else
	{
		AudioComponent->SetPaused(true);
	}
}

void AFirstPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFirstPersonCharacter::Look);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AFirstPersonCharacter::Pause);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFirstPersonCharacter::Move(const FInputActionValue& Value)
{
	if (Health == 0 || IsPaused) return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFirstPersonCharacter::Look(const FInputActionValue& Value)
{
	if (Health == 0 || IsPaused) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>() * AimSensitivity;

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFirstPersonCharacter::Pause(const FInputActionValue& Value)
{
	if (Health == 0 || IsPaused) return;

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);
	IsPaused = true;

	APlayerController* PlayerController = GetController<APlayerController>();

	if (PauseWidgetClass != nullptr)
	{
		if (PauseWidget == nullptr)
		{
			PauseWidget = CreateWidget<UUserWidget>(PlayerController, PauseWidgetClass);
		}
		
		PauseWidget->AddToViewport();
		PlayerController->bShowMouseCursor = true;
	}
}

void AFirstPersonCharacter::Resume()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	IsPaused = false;

	PauseWidget->RemoveFromViewport();

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = false;
	}
}

void AFirstPersonCharacter::Hurt(float Damage)
{
	if (Health == 0 || IsPaused) return;

	Health -= Damage;
	UGameplayStatics::PlaySoundAtLocation(this, HurtSound, GetActorLocation(), 1.0f);

	if (Health <= 0) {
		Health = 0;
		Death();
	}
}

void AFirstPersonCharacter::Death()
{
	APlayerController* PlayerController = GetController<APlayerController>();

	if (GameOverWidgetClass != nullptr)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(PlayerController, GameOverWidgetClass);
		GameOverWidget->AddToViewport();
		PlayerController->bShowMouseCursor = true;
	}

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);
	PlayerController->CustomTimeDilation = 0.0f;
}

void AFirstPersonCharacter::Heal(float HealthAmount)
{
	if (Health == 0 || IsPaused) return;

	Health += HealthAmount;

	if (Health > 100) {
		Health = 100;
	}
}
