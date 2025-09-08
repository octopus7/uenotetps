#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CoverTypes.h"
#include "CoverUser.generated.h"

class ACoverObjectActor;
class UCoverPointComponent;

UINTERFACE(BlueprintType)
class UCoverUser : public UInterface
{
    GENERATED_BODY()
};

class ICoverUser
{
    GENERATED_BODY()

public:
    // Receive a request to take cover at a specific point
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cover")
    void HandleTakeCoverRequest(ACoverObjectActor* CoverObject, UCoverPointComponent* Point, FCoverSolution Solution);

    // Notify when best cover changes (optional)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cover")
    void HandleBestCoverChanged(ACoverObjectActor* CoverObject, UCoverPointComponent* Point);

    // Notify when valid cover becomes available or is lost
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cover")
    void HandleCoverFound(ACoverObjectActor* CoverObject, UCoverPointComponent* Point, FCoverSolution Solution);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cover")
    void HandleCoverLost();
};

