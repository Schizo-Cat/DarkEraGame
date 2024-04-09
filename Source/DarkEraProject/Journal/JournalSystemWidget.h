// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "JournalSystemWidget.generated.h"

USTRUCT(BlueprintType)
struct FJournalEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Image;
};

UCLASS()
class DARKERAPROJECT_API UJournalSystemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddJournalEntry(FTableRowBase NewEntry);

	UFUNCTION(BlueprintCallable)
	void UnlockJournalEntry(int32 EntryIndex);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* DescriptionText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* JournalImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* JournalDataTable;

	UPROPERTY()
	TArray<FJournalEntry> JournalEntries;

	int32 CurrentEntryIndex = 0;

	virtual void NativeConstruct() override;
};
