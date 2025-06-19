// Fill out your copyright notice in the Description page of Project Settings.
#include "EnemyCatCharacter.h"

#include "EngineUtils.h"       // TActorIterator
#include "GameFramework/Actor.h"


#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AEnemyCatCharacter::AEnemyCatCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCatCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCatCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AEnemyCatCharacter::FireAtNearestEnemy()
{
    if (!ProjectileClass) return;

    AActor* NearestEnemy = nullptr;
    float MinDistance = FLT_MAX;
    FVector MyLocation = GetActorLocation();

    // ��� ���� ��ȸ
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Actor = *It;
        if (Actor && Actor->ActorHasTag("enemy"))
        {
            float Distance = FVector::Dist(MyLocation, Actor->GetActorLocation());
            if (Distance < MinDistance)
            {
                MinDistance = Distance;
                NearestEnemy = Actor;
            }
        }
    }

    if (NearestEnemy)
    {
        FVector MuzzleLocation = MyLocation + GetActorForwardVector() * 100.f;
        FVector TargetLocation = NearestEnemy->GetActorLocation();
        FVector Direction = (TargetLocation - MuzzleLocation).GetSafeNormal();

        // ȸ�� ���
        FRotator SpawnRotation = Direction.Rotation();

        // ���� �Ķ����
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnParams.Owner = this;

        // ����ü ����
        AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(
            ProjectileClass,
            MuzzleLocation,
            SpawnRotation,
            SpawnParams
        );

        // ProjectileMovementComponent�� �ִٸ� ���� ����
        if (SpawnedProjectile)
        {
            UProjectileMovementComponent* ProjMove = SpawnedProjectile->FindComponentByClass<UProjectileMovementComponent>();
            if (ProjMove)
            {
                ProjMove->Velocity = Direction * ProjMove->InitialSpeed;
            }
        }
    }
}
