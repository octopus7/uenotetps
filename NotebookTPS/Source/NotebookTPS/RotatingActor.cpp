// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingActor.h"

// Sets default values
ARotatingActor::ARotatingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    RotationSpeed = 90.0f; // �⺻ ȸ�� �ӵ� (�ʴ� 90��)
}

void ARotatingActor::BeginPlay()
{
    Super::BeginPlay();
}

void ARotatingActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ȸ�� ���� ���� (Yaw ���� ȸ��)
    FRotator DeltaRotation(0.f, RotationSpeed * DeltaTime, 0.f);
    AddActorLocalRotation(DeltaRotation);
}