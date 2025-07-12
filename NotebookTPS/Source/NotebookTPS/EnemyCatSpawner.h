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
    int32 MaxEnemyCount = 5;

    UPROPERTY(EditAnywhere)
    int32 SpawnedEnemyCount = 0;

    UPROPERTY(EditAnywhere)
    int32 MaxSpawnedEnemyCount = 4;


    UPROPERTY(EditAnywhere)
    float SpawnRadius = 1000.f;

    UPROPERTY(EditAnywhere)
    float Health = 100.f;


    UPROPERTY(EditAnywhere)
    float SpawnInterval = 3.f;


    UPROPERTY(EditAnywhere, Category = "Durability")
    float MaxDurability = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Durability")
    float CurrentDurability;

    // Called every frame
    virtual void Tick(float DeltaTime) override;
    // 데미지 받는 함수
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

private:





protected:
    void SpawnEnemy();
    FTimerHandle SpawnTimer;

};
