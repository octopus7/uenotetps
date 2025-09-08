#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "CoverTypes.generated.h"

UENUM(BlueprintType)
enum class ECoverHeight : uint8
{
    Low  UMETA(DisplayName = "Low"),
    High UMETA(DisplayName = "High")
};

USTRUCT(BlueprintType)
struct FCoverCandidateStatus
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Candidate")
    bool bWithinDistance = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Candidate")
    bool bWithinAngle = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Candidate")
    float Distance = 0.f;

    // Angle (deg) between Pawn forward and required facing toward cover (toward -Normal)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Candidate")
    float FacingAngleDeg = 180.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Candidate")
    bool bMeetsAll = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Candidate")
    bool bIsBest = false;
};

USTRUCT(BlueprintType)
struct FCoverSolution
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Solution")
    bool bValid = false;

    // Where the character should snap to at cover
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Solution")
    FVector SnapLocation = FVector::ZeroVector;

    // Facing at cover (aligned along the cover edge, looking along tangent)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Solution")
    FRotator SnapFacing = FRotator::ZeroRotator;

    // Optional lean positions (relative to world for simplicity)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Solution")
    FTransform LeftLeanTransform;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cover|Solution")
    FTransform RightLeanTransform;
};

// Utility to pick a debug color based on candidate status
static inline FColor CoverDebugColor(const FCoverCandidateStatus& Status)
{
    if (Status.bIsBest)
    {
        return FColor::Cyan;
    }
    if (Status.bMeetsAll)
    {
        return FColor::Green;
    }
    if (Status.bWithinDistance)
    {
        return FColor::Yellow;
    }
    return FColor::Red;
}

