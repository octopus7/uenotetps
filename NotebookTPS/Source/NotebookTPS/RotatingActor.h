// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingActor.generated.h"

UCLASS()
class NOTEBOOKTPS_API ARotatingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARotatingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 회전 속도 (Yaw 기준, 초당 도 단위 회전)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation", meta = (ExposeOnSpawn = "true"))
	float RotationSpeed;

	// 메시 표시용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

};
