// Fill out your copyright notice in the Description page of Project Settings.

#include "AlphaButton.h"

#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"

#pragma region >>> SAlphaButton <<<

void SAlphaButton::SetTextureData(UTexture2D* InTexture)
{
	TextureData = static_cast<FColor*>((InTexture->GetPlatformData()->Mips[0]).BulkData.Lock(LOCK_READ_ONLY));
	ImageHeight = InTexture->GetPlatformData()->SizeY;
	ImageWidth = InTexture->GetPlatformData()->SizeX;
	InTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
}

FReply SAlphaButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (TextureData)
	{
		if (CachedOverlap) 
		{
			return SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
		}
		return FReply::Unhandled();
	}
	return SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply SAlphaButton::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) 
{
	if (TextureData)
	{
		if (CachedOverlap)
		{
			return SButton::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
		}
		return FReply::Unhandled();
	}
	return SButton::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FReply SAlphaButton::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (TextureData)
	{
		if (CachedOverlap && IsPressed())
		{
			return SButton::OnMouseButtonUp(MyGeometry, MouseEvent);
		}
		return FReply::Unhandled();
	}
	return SButton::OnMouseButtonUp(MyGeometry, MouseEvent);
}

void SAlphaButton::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (TextureData)
	{
		if (CachedOverlap)
		{
			return SButton::OnMouseEnter(MyGeometry, MouseEvent);
		}
		else
		{
			if(IsMouseOverOpaquePixel( MouseEvent))
			{
				CachedOverlap = true;
				return SButton::OnMouseEnter(MyGeometry, MouseEvent);
			}
			else
			{
				CachedOverlap = false;
				return;
			}
		}
	}
	return SButton::OnMouseEnter(MyGeometry, MouseEvent);
}

void SAlphaButton::OnMouseLeave(const FPointerEvent& MouseEvent) 
{
	if(TextureData)
	{
		if (CachedOverlap)
		{
			CachedOverlap = false;
			return SButton::OnMouseLeave(MouseEvent);
		}
	}
	return SButton::OnMouseLeave(MouseEvent);
}

bool SAlphaButton::IsMouseOverOpaquePixel( const FPointerEvent & MouseEvent) const
{
	bool OverOpaque = true;
	
	const FGeometry MyGeometry = GetCachedGeometry();
	FVector2D LocalPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	LocalPosition.X = floor(LocalPosition.X);
	LocalPosition.Y = floor(LocalPosition.Y);
	LocalPosition /= MyGeometry.GetLocalSize();
	
	if (LocalPosition.X > 1.0f || LocalPosition.Y > 1.0f || LocalPosition.X<0.0f || LocalPosition.Y<0.0f)
	{
		return false;
	}
	
	
	if (!TextureData) 
	{
		OverOpaque = false;
	}
	else 
	{
		LocalPosition.X *= ImageWidth;
		LocalPosition.Y *= ImageHeight;
		const int BufferPosition = ((int)LocalPosition.Y * ImageWidth) + (int)LocalPosition.X;
		if (TextureData[BufferPosition].A <= AdvancedHitAlpha) 
			OverOpaque = false; 
	}
	
	return OverOpaque;
}

FReply SAlphaButton::OnMouseMove(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	if (!TextureData) 
		return SButton::OnMouseMove(MyGeometry, MouseEvent);

	if(IsMouseOverOpaquePixel(MouseEvent))
	{
		if (!CachedOverlap)
		{
			CachedOverlap = true;
			OnMouseEnter(MyGeometry, MouseEvent);
		}
	}
	else
	{
		if (CachedOverlap)
		{
			CachedOverlap = false;
			OnMouseLeave(MouseEvent);
		}
	}
	
	return FReply::Handled();
}


FCursorReply SAlphaButton::OnCursorQuery(const FGeometry & MyGeometry, const FPointerEvent & CursorEvent) const
{
	if (!IsHovered()) return FCursorReply::Unhandled();
	TOptional<EMouseCursor::Type> TheCursor = GetCursor();
	return (TheCursor.IsSet()) ? FCursorReply::Cursor(TheCursor.GetValue()) : FCursorReply::Unhandled();
}

#pragma endregion

#pragma region >>> UAlphaButton <<<

UAlphaButton::UAlphaButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  AdvancedHitTexture(nullptr),
	  AdvancedHitAlpha(0)
{
}

void UAlphaButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	if(MyButton.IsValid())
	{
		if(AdvancedHitTexture) (static_cast<SAlphaButton*>(MyButton.Get()))->SetTextureData(AdvancedHitTexture);
		(static_cast<SAlphaButton*>(MyButton.Get()))->SetAdvancedHitAlpha(AdvancedHitAlpha);
	}
}


TSharedRef<SWidget> UAlphaButton::RebuildWidget()
{
	const TSharedPtr<SAlphaButton> AlphaButton = SNew(SAlphaButton)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, SlateHandleClicked))
		.OnPressed(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandlePressed))
		.OnReleased(BIND_UOBJECT_DELEGATE(FSimpleDelegate, SlateHandleReleased))
		.OnHovered_UObject(this, &ThisClass::SlateHandleHovered)
		.OnUnhovered_UObject(this, &ThisClass::SlateHandleUnhovered)
		.ButtonStyle(&GetStyle())
		.ClickMethod(GetClickMethod())
		.TouchMethod(GetTouchMethod())
		.IsFocusable(GetIsFocusable())
		;

	if(AdvancedHitTexture) AlphaButton->SetTextureData(AdvancedHitTexture);
	
	AlphaButton->SetAdvancedHitAlpha(AdvancedHitAlpha);

	MyButton = AlphaButton;

	if (GetChildrenCount() > 0) Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());

	return MyButton.ToSharedRef();
}
#pragma endregion