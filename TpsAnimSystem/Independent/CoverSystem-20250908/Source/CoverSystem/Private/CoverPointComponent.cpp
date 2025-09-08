#include "CoverPointComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"

UCoverPointComponent::UCoverPointComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

FVector UCoverPointComponent::GetWorldNormal() const
{
    const FTransform& Xform = GetComponentTransform();
    return Xform.TransformVectorNoScale(LocalNormal).GetSafeNormal();
}

void UCoverPointComponent::EvaluateCandidate(const APawn* Pawn, FCoverCandidateStatus& OutStatus) const
{
    OutStatus = {};
    if (!Pawn)
    {
        return;
    }

    const FVector PointLocation = GetComponentLocation();
    const FVector WorldNormal = GetWorldNormal();
    const FVector PawnLocation = Pawn->GetActorLocation();
    const FVector PawnForward = Pawn->GetActorForwardVector();

    const float Distance = FVector::Dist2D(PawnLocation, PointLocation); // ignore Z by default for TPS cover
    OutStatus.Distance = Distance;
    OutStatus.bWithinDistance = (Distance <= AcceptDistance);

    // Required: Pawn looks toward -WorldNormal (facing the cover surface)
    const float Dot = FVector::DotProduct(PawnForward.GetSafeNormal(), (-WorldNormal).GetSafeNormal());
    const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f)));
    OutStatus.FacingAngleDeg = AngleDeg;
    OutStatus.bWithinAngle = (AngleDeg <= AcceptAngleDeg);

    OutStatus.bMeetsAll = OutStatus.bWithinDistance && OutStatus.bWithinAngle;
}

void UCoverPointComponent::BuildSolution(const APawn* Pawn, FCoverSolution& OutSolution) const
{
    OutSolution = {};
    if (!Pawn)
    {
        return;
    }

    const FVector PointLocation = GetComponentLocation();
    const FVector WorldNormal = GetWorldNormal();

    // Snap a bit inside toward the cover surface
    const FVector SnapLocation = PointLocation + (-WorldNormal) * SnapDistance;

    // Choose a facing roughly tangent to the cover edge; here we align yaw perpendicular to normal
    const FRotator Facing = (-WorldNormal).Rotation();

    OutSolution.bValid = true;
    OutSolution.SnapLocation = SnapLocation;
    OutSolution.SnapFacing = Facing;

    // Simple lean transforms (left/right) – offset along left/right from the point
    const FVector Right = FVector::CrossProduct(WorldNormal, FVector::UpVector).GetSafeNormal();
    const float LeanOffset = 25.f;
    OutSolution.LeftLeanTransform = FTransform(Facing, SnapLocation - Right * LeanOffset);
    OutSolution.RightLeanTransform = FTransform(Facing, SnapLocation + Right * LeanOffset);
}

void UCoverPointComponent::DrawDebug(UWorld* World, const FCoverCandidateStatus* OptionalStatus, bool bForceBest) const
{
    if (!World || !bDrawDebug)
    {
        return;
    }

    FCoverCandidateStatus Status;
    if (OptionalStatus)
    {
        Status = *OptionalStatus;
    }
    Status.bIsBest = Status.bIsBest || bForceBest;

    const FVector P = GetComponentLocation();
    const FVector N = GetWorldNormal();
    const FColor C = CoverDebugColor(Status);

    DrawDebugSphere(World, P, DebugPointRadius, 12, C, false, 0.f, 0, 1.5f);
    DrawDebugDirectionalArrow(World, P, P + N * DebugNormalLength, 10.f, C, false, 0.f, 0, 1.5f);
}

