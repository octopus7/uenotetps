// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCatSpawner.h"

#include "EngineUtils.h"

// Sets default values
AEnemyCatSpawner::AEnemyCatSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCatSpawner::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(SpawnTimer, this, &AEnemyCatSpawner::SpawnEnemy, SpawnInterval, true);
}

// Called every frame
void AEnemyCatSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyCatSpawner::SpawnEnemy()
{
    if (!EnemyClass) return;

    int32 ExistingEnemies = 0;
    for (TActorIterator<AEnemyCatCharacter> It(GetWorld()); It; ++It)
    {
        ExistingEnemies++;
    }

    if (ExistingEnemies >= MaxEnemyCount) return;

    FVector Origin = GetActorLocation();
    FVector Offset = FMath::VRand() * SpawnRadius;
    FVector SpawnLocation = Origin + Offset;
    FRotator SpawnRotation = FRotator::ZeroRotator;

    GetWorld()->SpawnActor<AEnemyCatCharacter>(EnemyClass, SpawnLocation, SpawnRotation);
}


