#include "AutoJumpingCube.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/CollisionProfile.h"
#include "TimerManager.h"

AAutoJumpingCube::AAutoJumpingCube()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    SetRootComponent(Mesh);
    Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

    // Load Engine basic cube
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        Mesh->SetStaticMesh(CubeMesh.Object);
    }
}

void AAutoJumpingCube::BeginPlay()
{
    Super::BeginPlay();

    BaseLocation = GetActorLocation();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            JumpTimerHandle,
            this,
            &AAutoJumpingCube::TriggerJump,
            JumpIntervalSeconds,
            true,
            JumpIntervalSeconds // initial delay equals interval
        );
    }
}

void AAutoJumpingCube::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!bIsJumping)
    {
        return;
    }

    JumpElapsed += DeltaSeconds;

    const float Alpha = FMath::Clamp(JumpElapsed / JumpPhaseDuration, 0.0f, 1.0f);
    float OffsetZ = 0.0f;

    if (bGoingUp)
    {
        // Ease out going up
        OffsetZ = FMath::InterpEaseOut(0.0f, JumpHeight, Alpha, 2.0f);
    }
    else
    {
        // Ease in coming down
        OffsetZ = FMath::InterpEaseIn(JumpHeight, 0.0f, Alpha, 2.0f);
    }

    SetActorLocation(BaseLocation + FVector(0.0f, 0.0f, OffsetZ));

    if (JumpElapsed >= JumpPhaseDuration)
    {
        if (bGoingUp)
        {
            // Switch to falling phase
            bGoingUp = false;
            JumpElapsed = 0.0f;
        }
        else
        {
            // Landed
            bIsJumping = false;
            JumpElapsed = 0.0f;
            SetActorLocation(BaseLocation);
        }
    }
}

void AAutoJumpingCube::TriggerJump()
{
    if (bIsJumping)
    {
        // Skip if still mid-jump
        return;
    }

    BaseLocation = GetActorLocation();
    bIsJumping = true;
    bGoingUp = true;
    JumpElapsed = 0.0f;
}
