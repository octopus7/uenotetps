// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCatCharacter.h"
#include "EnemyCatSpawner.generated.h"


UCLASS()
class NOTEBOOKTPS_API AEnemyCatSpawner : public AActor
{
    GENERATED_BODY()

public:

    AEnemyCatSpawner();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere)
    TSubclassOf<AEnemyCatCharacter> EnemyClass;

    UPROPERTY(EditAnywhere)
    int32 MaxEnemyCount = 10;

    UPROPERTY(EditAnywhere)
    float SpawnRadius = 1000.f;

    UPROPERTY(EditAnywhere)
    float SpawnInterval = 3.f;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    void SpawnEnemy();
    FTimerHandle SpawnTimer;

};
