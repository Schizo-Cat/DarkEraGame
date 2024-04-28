// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MoviePlayer.h" 
#include "Slate.h"

#include "SpaceGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DARKERAPROJECT_API USpaceGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(DisplayName = "BeginLoadingScreen"), Category = "Loading Screen")
	void BeginLoadingScreen(const FString& MapName);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(DisplayName = "EndLoadingScreen"), Category = "Loading Screen")
	void EndLoadingScreen(UWorld* IsLoadedWorld);

	UPROPERTY(EditDefaultsOnly, Category = "Loading Screen")
	TSubclassOf<class UUserWidget> LoadingScreenWidgetClass;
    
	UUserWidget* LoadingScreenWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
	bool UseMovies;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading Screen")
	TArray<FString> StringPaths;
};
