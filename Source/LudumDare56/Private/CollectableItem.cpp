#include "CollectableItem.h"
#include "Components/StaticMeshComponent.h"
#include "FirstPersonCharacter.h"
#include <Kismet/GameplayStatics.h>

ACollectableItem::ACollectableItem()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = SceneComponent;

    CollectableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollectableMesh"));
    CollectableMesh->SetupAttachment(RootComponent);

    CollectableMesh->SetGenerateOverlapEvents(true);
    CollectableMesh->OnComponentBeginOverlap.AddDynamic(this, &ACollectableItem::OnOverlapBegin);
}

void ACollectableItem::BeginPlay()
{
    Super::BeginPlay();
}

void ACollectableItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Rotation (rotate around Z-axis)
    FRotator NewRotation = GetActorRotation();
    NewRotation.Yaw += DeltaTime * 50.0f;  // Adjust 50.0f to control the speed of rotation
    SetActorRotation(NewRotation);

    // Up and down movement (sinusoidal motion)
    FVector NewLocation = GetActorLocation();
    float DeltaHeight = FMath::Sin(GetWorld()->GetTimeSeconds() * 2.0f); // Oscillate with a period of roughly 3.14 seconds
    NewLocation.Z += DeltaHeight * 20.0f * DeltaTime;  // Adjust 20.0f to control the amplitude of the movement
    SetActorLocation(NewLocation);
}

void ACollectableItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA(AFirstPersonCharacter::StaticClass()))
    {
        UGameplayStatics::PlaySoundAtLocation(this, CollectSound, GetActorLocation(), 1.0f);
        ApplyEffect(OtherActor);
        Destroy();
    }
}

void ACollectableItem::ApplyEffect(AActor* PlayerActor)
{
    // Default implementation does nothing; derived classes will override this
}
