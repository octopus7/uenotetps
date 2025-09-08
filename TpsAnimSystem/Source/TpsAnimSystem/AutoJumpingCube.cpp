#include "AutoJumpingCube.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"
#include "Engine/CollisionProfile.h"
#include "TimerManager.h"
#include "Components/PrimitiveComponent.h"

AAutoJumpingCube::AAutoJumpingCube()
{
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    SetRootComponent(Mesh);
    Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
    Mesh->SetSimulatePhysics(true);
    Mesh->SetEnableGravity(true);
    Mesh->SetNotifyRigidBodyCollision(true);

    // Load Engine basic cube
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        Mesh->SetStaticMesh(CubeMesh.Object);
    }

    // Try apply preferred engine material (highest priority), otherwise fall back.
    // If none of the candidates exist, do nothing (keep default material).
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> PreferredMat(TEXT("/Engine/TemplateResources/MI_Template_BaseOrange_Metal.MI_Template_BaseOrange_Metal"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlueMat1(TEXT("/Engine/EditorMaterials/PersonaBoneUnselectedMaterial.PersonaBoneUnselectedMaterial"));
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> BlueMat2(TEXT("/Engine/EngineDebugMaterials/VertexColorViewMode_BlueOnly.VertexColorViewMode_BlueOnly"));

    if (PreferredMat.Succeeded())
    {
        Mesh->SetMaterial(0, PreferredMat.Object);
    }
    else if (BlueMat1.Succeeded())
    {
        Mesh->SetMaterial(0, BlueMat1.Object);
    }
    else if (BlueMat2.Succeeded())
    {
        Mesh->SetMaterial(0, BlueMat2.Object);
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

    if (Mesh)
    {
        Mesh->OnComponentHit.AddDynamic(this, &AAutoJumpingCube::OnMeshHit);
    }
}

void AAutoJumpingCube::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // If simulating physics, let physics handle movement; skip manual animation.
    if (Mesh && Mesh->IsSimulatingPhysics())
    {
        return;
    }

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
    if (Mesh && Mesh->IsSimulatingPhysics())
    {
        if (bIsJumping)
        {
            return;
        }
        // Apply upward velocity change to reach approximately JumpHeight (cm)
        const float Gravity = FMath::Abs(GetWorld()->GetGravityZ()); // cm/s^2
        const float DesiredHeight = JumpHeight; // cm
        const float DeltaV = FMath::Sqrt(FMath::Max(0.f, 2.f * Gravity * DesiredHeight)); // cm/s
        Mesh->AddImpulse(FVector(0.f, 0.f, DeltaV), NAME_None, true); // bVelChange=true => delta-V
        bIsJumping = true;
        return;
    }

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

void AAutoJumpingCube::OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                 FVector NormalImpulse, const FHitResult& Hit)
{
    if (!(Mesh && Mesh->IsSimulatingPhysics()))
    {
        return;
    }

    const FVector Vel = Mesh->GetComponentVelocity();
    const bool bMovingDown = Vel.Z <= 0.f;
    const bool bUpwardSurface = Hit.ImpactNormal.Z > 0.3f;
    if (bIsJumping && bMovingDown && bUpwardSurface)
    {
        bIsJumping = false;
    }
}
