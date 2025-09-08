#include "CoverDetectorComponent.h"
#include "CoverObjectActor.h"
#include "CoverPointComponent.h"
#include "CoverUser.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"

UCoverDetectorComponent::UCoverDetectorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UCoverDetectorComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCoverDetectorComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    ScanForCover();
}

void UCoverDetectorComponent::GetBestCover(ACoverObjectActor*& OutCoverObject, UCoverPointComponent*& OutPoint, FCoverCandidateStatus& OutStatus) const
{
    OutCoverObject = BestCoverObject.Get();
    OutPoint = BestCoverPoint.Get();
    OutStatus = BestStatus;
}

void UCoverDetectorComponent::RequestTakeCover()
{
    ACoverObjectActor* Obj = BestCoverObject.Get();
    UCoverPointComponent* Pt = BestCoverPoint.Get();
    if (!Obj || !Pt)
    {
        return;
    }

    if (!BestStatus.bMeetsAll)
    {
        return; // require valid candidate
    }

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    FCoverSolution Solution;
    Pt->BuildSolution(OwnerPawn, Solution);
    if (!Solution.bValid)
    {
        return;
    }

    OnTakeCoverRequested.Broadcast(Obj, Solution);

    // Optional interface callback
    if (GetOwner()->GetClass()->ImplementsInterface(UCoverUser::StaticClass()))
    {
        ICoverUser::Execute_HandleTakeCoverRequest(GetOwner(), Obj, Pt, Solution);
    }
}

void UCoverDetectorComponent::RequestLeaveCover()
{
    OnLeaveCoverRequested.Broadcast();
}

void UCoverDetectorComponent::ScanForCover()
{
    UWorld* World = GetWorld();
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!World || !OwnerPawn)
    {
        return;
    }

    const FVector PawnLoc = OwnerPawn->GetActorLocation();

    TWeakObjectPtr<ACoverObjectActor> NewBestObj = nullptr;
    TWeakObjectPtr<UCoverPointComponent> NewBestPt = nullptr;
    FCoverCandidateStatus NewBestStatus{};
    float BestScore = TNumericLimits<float>::Max();

    // Iterate all cover objects in the current world
    for (TActorIterator<ACoverObjectActor> It(World); It; ++It)
    {
        ACoverObjectActor* Obj = *It;
        if (!Obj)
        {
            continue;
        }

        const TArray<UCoverPointComponent*>& Points = Obj->GetCoverPoints();
        for (UCoverPointComponent* Pt : Points)
        {
            if (!Pt)
            {
                continue;
            }

            // Quick cull by scan radius (2D)
            const float Dist2D = FVector::Dist2D(PawnLoc, Pt->GetComponentLocation());
            if (Dist2D > ScanRadius)
            {
                if (bDrawDebug && !bDrawOnlyBest)
                {
                    FCoverCandidateStatus S; // default => Red
                    Pt->DrawDebug(World, &S, false);
                }
                continue;
            }

            FCoverCandidateStatus Status;
            Pt->EvaluateCandidate(OwnerPawn, Status);

            // Compute a score: prefer smallest distance; penalize angle overflow
            float Score = Status.Distance;
            if (!Status.bWithinAngle)
            {
                Score += 1000.f; // deprioritize failing angle
            }

            // Optionally require valid thresholds
            if (bRequireMeetsAllForBest && !Status.bMeetsAll)
            {
                // Still draw debug for nearby points
                if (bDrawDebug && !bDrawOnlyBest)
                {
                    Pt->DrawDebug(World, &Status, false);
                }
                continue;
            }

            if (Score < BestScore)
            {
                BestScore = Score;
                NewBestObj = Obj;
                NewBestPt = Pt;
                NewBestStatus = Status;
            }

            if (bDrawDebug && !bDrawOnlyBest)
            {
                Pt->DrawDebug(World, &Status, false);
            }
        }
    }

    // Draw best only highlight
    if (bDrawDebug && NewBestPt.IsValid())
    {
        FCoverCandidateStatus BestVis = NewBestStatus;
        BestVis.bIsBest = true;
        NewBestPt->DrawDebug(World, &BestVis, true);
    }

    NotifyChanges(NewBestObj, NewBestPt, NewBestStatus);
}

void UCoverDetectorComponent::NotifyChanges(const TWeakObjectPtr<ACoverObjectActor>& NewObj,
                                             const TWeakObjectPtr<UCoverPointComponent>& NewPt,
                                             const FCoverCandidateStatus& NewStatus)
{
    const bool bBestChanged = (NewObj != BestCoverObject) || (NewPt != BestCoverPoint);
    const bool bHadValidBefore = bHadValidCover;
    const bool bHasValidNow = NewStatus.bMeetsAll && NewObj.IsValid() && NewPt.IsValid();

    BestCoverObject = NewObj;
    BestCoverPoint = NewPt;
    BestStatus = NewStatus;
    bHadValidCover = bHasValidNow;

    if (bBestChanged)
    {
        OnBestCoverChanged.Broadcast(BestCoverObject.Get(), BestCoverPoint.Get());

        // Optional interface callback
        if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UCoverUser::StaticClass()))
        {
            ICoverUser::Execute_HandleBestCoverChanged(GetOwner(), BestCoverObject.Get(), BestCoverPoint.Get());
        }
    }

    if (!bHadValidBefore && bHasValidNow)
    {
        // Found valid cover
        ACoverObjectActor* Obj = BestCoverObject.Get();
        UCoverPointComponent* Pt = BestCoverPoint.Get();
        FCoverSolution Solution;
        if (Obj && Pt)
        {
            APawn* OwnerPawn = Cast<APawn>(GetOwner());
            Pt->BuildSolution(OwnerPawn, Solution);
        }
        OnCoverFound.Broadcast(Obj, Pt);
        if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UCoverUser::StaticClass()))
        {
            ICoverUser::Execute_HandleCoverFound(GetOwner(), Obj, Pt, Solution);
        }
    }
    else if (bHadValidBefore && !bHasValidNow)
    {
        // Lost valid cover
        OnCoverLost.Broadcast();
        if (GetOwner() && GetOwner()->GetClass()->ImplementsInterface(UCoverUser::StaticClass()))
        {
            ICoverUser::Execute_HandleCoverLost(GetOwner());
        }
    }
}

