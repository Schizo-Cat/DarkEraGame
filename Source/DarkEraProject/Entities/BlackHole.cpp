// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHole.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABlackHole::ABlackHole()
{
	PrimaryActorTick.bCanEverTick = true;

	BlackHoleComponent = CreateDefaultSubobject<USphereComponent>(TEXT("BlackHoleComponent"));
	RootComponent = BlackHoleComponent;
}

void ABlackHole::BeginPlay()
{
	Super::BeginPlay();

	CurrentMass = InitialMass;

	PlayerController = Cast<ASpacePlayerController>(GetWorld()->GetFirstPlayerController());
	
	// Set the initial radius of the sphere based on the initial mass
	float InitialRadius = FMath::Sqrt(CurrentMass) / 10.0f; // Scaled down for visualization
	BlackHoleComponent->InitSphereRadius(InitialRadius);
}

void ABlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(PlayerController)
	{
		if (CurrentMass > 0.0f && !PlayerController->IsTimeFrozen)
		{
			CurrentMass -= MassLossRate * DeltaTime * PlayerController->timeSpeed;

			if (CurrentMass <= 0.0f)
			{
				OnDeath();
			}
			else
			{
				// Update the radius of the sphere based on the current mass
				float CurrentScale = InitialScale * FMath::Sqrt(CurrentMass / InitialMass);
				SetActorScale3D(FVector(CurrentScale));
			}
		}
	}
}

float ABlackHole::GetMass() const
{
	return CurrentMass;
}

void ABlackHole::OnDeath()
{
	TurnOffGameSound();
	RemoveAllUI();
	MakeScreenGoDark();

	Destroy();
}

void ABlackHole::TurnOffGameSound()
{
	UGameplayStatics::SetSoundMixClassOverride(GetWorld()->GetFirstPlayerController(), nullptr, nullptr,1.0f, 0.0f, 0.0f, true);
}

void ABlackHole::RemoveAllUI()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ConsoleCommand("ShowHUD false");
}

void ABlackHole::MakeScreenGoDark()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ConsoleCommand("PostProcessAffectsScene 0");
}
