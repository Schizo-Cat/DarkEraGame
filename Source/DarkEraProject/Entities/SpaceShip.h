// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlackHole.h"
#include "GameFramework/Actor.h"
#include "SpaceShip.generated.h"

UCLASS()
class DARKERAPROJECT_API ASpaceShip : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpaceShip();

	virtual void Tick(float DeltaTime) override;
	ABlackHole* GetBlackHoleReference(UObject* WorldContextObject);

	ABlackHole* MainBlackHole;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float ShakeIntensity = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float RotationSpeed = 180.0f;

protected:
	virtual void BeginPlay() override;

private:
	float ShakeTimelineProgress = 0.0f;
	float RotationTimelineProgress = 0.0f;

};
