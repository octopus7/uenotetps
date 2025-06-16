// Fill out your copyright notice in the Description page of Project Settings.


#include "ChickenActor.h"

// Sets default values
AChickenActor::AChickenActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AChickenActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AChickenActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

