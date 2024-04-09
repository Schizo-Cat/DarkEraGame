// Fill out your copyright notice in the Description page of Project Settings.


#include "JournalSystemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UJournalSystemWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Bind to widgets
    NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("NameText")));
    DescriptionText = Cast<UTextBlock>(GetWidgetFromName(TEXT("DescriptionText")));
    JournalImage = Cast<UImage>(GetWidgetFromName(TEXT("JournalImage")));

    // Load journal entries from DataTable
    if (JournalDataTable)
    {
        TArray<FName> RowNames = JournalDataTable->GetRowNames();
        for (auto& RowName : RowNames)
        {
            FJournalEntry* Entry = JournalDataTable->FindRow<FJournalEntry>(RowName, TEXT(""));
            if (Entry)
            {
                JournalEntries.Add(*Entry);
            }
        }
    }

    // Initialize first entry
    if (JournalEntries.Num() > 0)
    {
        NameText->SetText(FText::FromString(JournalEntries[CurrentEntryIndex].Name));
        DescriptionText->SetText(FText::FromString(JournalEntries[CurrentEntryIndex].Description));
        JournalImage->SetBrushFromTexture(JournalEntries[CurrentEntryIndex].Image);
    }
}

void UJournalSystemWidget::AddJournalEntry(FTableRowBase NewEntry)
{
    //JournalEntries.Add(NewEntry);

    // Update DataTable (assuming it's initialized)
    // if (JournalDataTable)
    // {
    //     FName EntryName = FName(*FString::FromInt(JournalEntries.Num() - 1));
    //     JournalDataTable->AddRow(EntryName, NewEntry);
    // }
}

void UJournalSystemWidget::UnlockJournalEntry(int32 EntryIndex)
{
    if (EntryIndex >= 0 && EntryIndex < JournalEntries.Num())
    {
        CurrentEntryIndex = EntryIndex;
        NameText->SetText(FText::FromString(JournalEntries[CurrentEntryIndex].Name));
        DescriptionText->SetText(FText::FromString(JournalEntries[CurrentEntryIndex].Description));
        JournalImage->SetBrushFromTexture(JournalEntries[CurrentEntryIndex].Image);
    }
}