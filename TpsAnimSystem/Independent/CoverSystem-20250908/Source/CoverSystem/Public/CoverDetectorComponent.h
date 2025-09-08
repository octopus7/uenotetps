#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoverTypes.h"
#include "CoverDetectorComponent.generated.h"

class ACoverObjectActor;
class UCoverPointComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBestCoverChanged, ACoverObjectActor*, CoverObject, UCoverPointComponent*, CoverPoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCoverFound, ACoverObjectActor*, CoverObject, UCoverPointComponent*, CoverPoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoverLost);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTakeCoverRequested, ACoverObjectActor*, CoverObject, FCoverSolution, Solution);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeaveCoverRequested);

/**
 * Actor component that scans for nearby cover points and selects the best candidate.
 * Emits delegates so owning Pawn/Character can respond without tight coupling.
 */
UCLASS(ClassGroup=(Cover), meta=(BlueprintSpawnableComponent))
class UCoverDetectorComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCoverDetectorComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Scan")
    float ScanRadius = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Debug")
    bool bDrawDebug = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Debug")
    bool bDrawOnlyBest = false;

    // If true, require candidate's thresholds (distance+angle) to be satisfied for best selection
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover|Scan")
    bool bRequireMeetsAllForBest = false;

    UPROPERTY(BlueprintAssignable, Category = "Cover|Event")
    FOnBestCoverChanged OnBestCoverChanged;

    UPROPERTY(BlueprintAssignable, Category = "Cover|Event")
    FOnCoverFound OnCoverFound;

    UPROPERTY(BlueprintAssignable, Category = "Cover|Event")
    FOnCoverLost OnCoverLost;

    UPROPERTY(BlueprintAssignable, Category = "Cover|Event")
    FOnTakeCoverRequested OnTakeCoverRequested;

    UPROPERTY(BlueprintAssignable, Category = "Cover|Event")
    FOnLeaveCoverRequested OnLeaveCoverRequested;

public:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Get current best cover point
    UFUNCTION(BlueprintCallable, Category = "Cover")
    void GetBestCover(ACoverObjectActor*& OutCoverObject, UCoverPointComponent*& OutPoint, FCoverCandidateStatus& OutStatus) const;

    // Request taking cover at the current valid best point (emits delegate and optional interface callback)
    UFUNCTION(BlueprintCallable, Category = "Cover")
    void RequestTakeCover();

    // Request leaving cover (just emits delegate)
    UFUNCTION(BlueprintCallable, Category = "Cover")
    void RequestLeaveCover();

private:
    TWeakObjectPtr<ACoverObjectActor> BestCoverObject;
    TWeakObjectPtr<UCoverPointComponent> BestCoverPoint;
    FCoverCandidateStatus BestStatus{};
    bool bHadValidCover = false;

    void ScanForCover();
    void NotifyChanges(const TWeakObjectPtr<ACoverObjectActor>& NewObj, const TWeakObjectPtr<UCoverPointComponent>& NewPt, const FCoverCandidateStatus& NewStatus);
};

