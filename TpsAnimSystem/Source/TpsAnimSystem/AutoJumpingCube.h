#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

class UPrimitiveComponent;

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

    UFUNCTION()
    void OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                   FVector NormalImpulse, const FHitResult& Hit);

    // Internal tuning (kept private; not required to expose)
    float JumpHeight = 200.0f;           // units (non-physics fallback)
    float JumpPhaseDuration = 0.22f;     // seconds for up or down phase (non-physics fallback)
    // Physics jump target height (cm). Used to compute delta-V each jump.
    // Matches non-physics fallback height for consistent behavior.
    // Note: 내부 값이며 필요 시 노출 가능.
};
