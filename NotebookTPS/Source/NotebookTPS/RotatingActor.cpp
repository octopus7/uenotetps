// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingActor.h"

// Sets default values
ARotatingActor::ARotatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    RotationSpeed = 90.0f; // 기본 회전 속도 (초당 90도)
}

void ARotatingActor::BeginPlay()
{
    Super::BeginPlay();
}

void ARotatingActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 회전 벡터 생성 (Yaw 기준 회전)
    FRotator DeltaRotation(0.f, RotationSpeed * DeltaTime, 0.f);
    AddActorLocalRotation(DeltaRotation);
}