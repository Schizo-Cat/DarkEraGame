// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "DarkEraProject/SpacePlayerController.h"
#include "GameFramework/Actor.h"
#include "BlackHole.generated.h"

UCLASS()
class DARKERAPROJECT_API ABlackHole : public AActor
{
	GENERATED_BODY()

public:
	ABlackHole();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	float GetMass() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackHole")
	float InitialMass = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackHole")
	float MassLossRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackHole")
	float InitialScale = 1.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	USphereComponent* BlackHoleComponent;

	ASpacePlayerController* PlayerController;
	UFUNCTION()
	void OnDeath();

	UFUNCTION()
	void TurnOffGameSound();

	UFUNCTION()
	void RemoveAllUI();

	UFUNCTION()
	void MakeScreenGoDark();

	float CurrentMass;
};
