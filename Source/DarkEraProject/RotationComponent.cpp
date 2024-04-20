// Fill out your copyright notice in the Description page of Project Settings.


#include "RotationComponent.h"

#include "Entities/SpaceShip.h"

// Sets default values for this component's properties
URotationComponent::URotationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URotationComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = Cast<ASpacePlayerController>(GetWorld()->GetFirstPlayerController());
}

void URotationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!PlayerController->IsTimeFrozen)
	{
		ShakeTimelineProgress += DeltaTime;
		FVector NewLocation = GetOwner()->GetActorLocation();
		NewLocation.X = FMath::Sin(ShakeTimelineProgress * 20.0f) * ShakeIntensity + GetOwner()->GetActorLocation().X;
		NewLocation.Y = FMath::Sin(ShakeTimelineProgress * 20.0f) * ShakeIntensity + GetOwner()->GetActorLocation().Y;
		NewLocation.Z = FMath::Sin(ShakeTimelineProgress * 20.0f) * ShakeIntensity + GetOwner()->GetActorLocation().Z;
		GetOwner()->SetActorLocation(NewLocation);

		if(Cast<ASpaceShip>(GetOwner())->MainBlackHole)
		{
			ABlackHole* MainBlackHole = Cast<ASpaceShip>(GetOwner())->MainBlackHole;
			RotationSpeed =  MainBlackHole->GetMass()/10;
			RotationTimelineProgress += DeltaTime;
			FRotator NewRotation = GetOwner()->GetActorRotation();
			//NewRotation.Yaw += RotationSpeed * DeltaTime;
			NewRotation.Roll += RotationSpeed * DeltaTime * PlayerController->timeSpeed;
			//NewRotation.Pitch += RotationSpeed * DeltaTime;
			GetOwner()->SetActorRotation(NewRotation);
		}
	}
}

