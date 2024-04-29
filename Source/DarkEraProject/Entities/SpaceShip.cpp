// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceShip.h"

#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpaceShip::ASpaceShip()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraComponent"));
	RootComponent = StaticMeshComponent;
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ASpaceShip::BeginPlay()
{
	Super::BeginPlay();
	MainBlackHole = GetBlackHoleReference(this);
	SetActorTickEnabled(true);
}

// Called every frame
void ASpaceShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ABlackHole* ASpaceShip::GetBlackHoleReference(UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (World)
		{
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(World, ABlackHole::StaticClass(), FoundActors);
			if (FoundActors.Num() > 0)
			{
				return Cast<ABlackHole>(FoundActors[0]);
			}
		}
	}

	return nullptr;
}

