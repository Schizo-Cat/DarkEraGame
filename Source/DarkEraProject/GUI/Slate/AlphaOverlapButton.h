// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Components/Button.h"
#include "AlphaOverlapButton.generated.h"

/**
 * 
 */

class DARKERAPROJECT_API SAlphaOverlapButton : public SButton {
	
public:

	TArray<uint8> TextureData;

	int32 ImageWidth=0;

	int32 ImageHeight=0;
	
	int AdvancedHitAlpha;

	TArray<TSharedPtr<SAlphaOverlapButton>> OverlapButtons;

	int  SiblingIndex;

	bool CachedOverlap;

	SAlphaOverlapButton() : ImageWidth(0), ImageHeight(0), AdvancedHitAlpha(0),
	                 SiblingIndex(0),
	                 CachedOverlap(false)
	{
	}

	void SetTextureData(UTexture2D* InTexture);
	
	void SetAdvancedHitAlpha(const int InAlpha) 
	{ AdvancedHitAlpha = InAlpha; }

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	virtual TSharedPtr<IToolTip> GetToolTip() override
	{ 
		return (IsHovered() ? SWidget::GetToolTip() : nullptr);
	}

	bool IsMouseOverOpaquePixel( const FPointerEvent & MouseEvent) const;

	
};

UCLASS()
class DARKERAPROJECT_API UAlphaOverlapButton : public UButton
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AdvancedHitTest") 
	UTexture2D* AdvancedHitTexture;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AdvancedHitTest", meta = (ClampMin = "0.0", ClampMax = "255.0", UIMin = "0.0", UIMax = "255.0")) 
	int AdvancedHitAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdvancedHitTest")
	TArray<TObjectPtr<UAlphaOverlapButton>> OverlapButtons;
	
	UFUNCTION(BlueprintCallable, Category = "AdvancedHitTest") void SetAdvancedHitTexture(UTexture2D* InTexture) 
	{
		AdvancedHitTexture = InTexture;
		if (MyButton.IsValid()) 
		(static_cast<SAlphaOverlapButton*>(MyButton.Get()))->SetTextureData(AdvancedHitTexture);
	}
	UFUNCTION(BlueprintCallable, Category = "AdvancedHitTest") void SetAdvancedHitAlpha(const int InAlpha)
	{
		AdvancedHitAlpha = InAlpha;
		if (MyButton.IsValid()) 
			(static_cast<SAlphaOverlapButton*>(MyButton.Get()))->SetAdvancedHitAlpha(AdvancedHitAlpha);
	}
	UFUNCTION(BlueprintCallable, Category = "AdvancedHitTest")
	void SetOverlappedButtons(TArray<UAlphaOverlapButton*> Buttons)
	{
		OverlapButtons = Buttons;
	}

	UFUNCTION(BlueprintCallable, Category = "AdvancedHitTest") 
	int GetSibIndex() const
	{
		return SiblingIndex;
	}

	UFUNCTION(BlueprintCallable, Category = "AdvancedHitTest") 
	void SetSibIndex(int Index)
	{
		SiblingIndex=Index;
		(static_cast<SAlphaOverlapButton*>(MyButton.Get()))->SiblingIndex = SiblingIndex;
	}

	TSharedPtr<SButton> GetCacheSButton()
	{
		return MyButton;
	}

	virtual void SynchronizeProperties() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	
	int  SiblingIndex;
};
