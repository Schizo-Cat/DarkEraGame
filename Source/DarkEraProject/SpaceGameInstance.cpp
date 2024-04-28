// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceGameInstance.h"

#include "Blueprint/UserWidget.h"

void USpaceGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &USpaceGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &USpaceGameInstance::EndLoadingScreen);
}

void USpaceGameInstance::BeginLoadingScreen_Implementation(const FString& MapName)
{
	if(!IsRunningDedicatedServer())
	{
		FLoadingScreenAttributes LoadingScreen;

		if(UseMovies)
		{
			LoadingScreen.MoviePaths = StringPaths;
		}

		LoadingScreenWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingScreenWidgetClass);
		if (LoadingScreenWidget)
		{
			LoadingScreen.WidgetLoadingScreen = LoadingScreenWidget->TakeWidget();
			//LoadingScreenWidget->AddToViewport();
		}

		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	}
}

void USpaceGameInstance::EndLoadingScreen_Implementation(UWorld* IsLoadedWorld)
{
}
