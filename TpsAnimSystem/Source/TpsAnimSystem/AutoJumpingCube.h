#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutoJumpingCube.generated.h"

UCLASS(Blueprintable)
class TPSANIMSYSTEM_API AAutoJumpingCube : public AActor
{
    GENERATED_BODY()

public:
    AAutoJumpingCube();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* Mesh;

    // Jump interval (seconds). Default: 2.0. Exposed for adjustment.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.05", UIMin = "0.05", Units = "s"))
    float JumpIntervalSeconds = 2.0f;

    // Internal jump state
    bool bIsJumping = false;
    bool bGoingUp = false;
    float JumpElapsed = 0.0f;
    FVector BaseLocation = FVector::ZeroVector;

    FTimerHandle JumpTimerHandle;

    void TriggerJump();

    // Internal tuning (kept private; not required to expose)
    float JumpHeight = 200.0f;           // units
    float JumpPhaseDuration = 0.22f;     // seconds for up or down phase
};

