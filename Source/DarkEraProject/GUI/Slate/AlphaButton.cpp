// Fill out your copyright notice in the Description page of Project Settings.

#include "AlphaButton.h"

#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"

#pragma region >>> SAlphaButton <<<

void SAlphaButton::SetTextureData(UTexture2D* InTexture)
{
	if(InTexture)
	{
		// Lock the texture to read its data
		FTexture2DMipMap& MipMap = InTexture->GetPlatformData()->Mips[0];
		FColor* ColorData = static_cast<FColor*>(MipMap.BulkData.Lock(LOCK_READ_ONLY));
		
		
		// Get the size of the texture
		ImageWidth = MipMap.SizeX;
		ImageHeight = MipMap.SizeY;
		
		for(int Y = 0; Y < ImageHeight; Y++)
		{
			for(int X = 0; X<ImageWidth; X++)
			{
				TextureData.Add(ColorData[(ImageWidth * Y) + X].A);
			}
		}
		
		// Unlock the texture
		MipMap.BulkData.Unlock();
	}
}

FReply SAlphaButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!TextureData.IsEmpty())
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
	if (!TextureData.IsEmpty())
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
	if (!TextureData.IsEmpty())
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
	if (!TextureData.IsEmpty())
	{
		if(IsMouseOverOpaquePixel(MouseEvent))
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
	return SButton::OnMouseEnter(MyGeometry, MouseEvent);
}

void SAlphaButton::OnMouseLeave(const FPointerEvent& MouseEvent) 
{
	if(!TextureData.IsEmpty())
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
	if (TextureData.IsEmpty()) return false;
	
	
	const FGeometry MyGeometry = GetCachedGeometry();
	FVector2D LocalPosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	LocalPosition.X = floor(LocalPosition.X);
	LocalPosition.Y = floor(LocalPosition.Y);
	LocalPosition /= MyGeometry.GetLocalSize();
	
	if (LocalPosition.X > 1.0f || LocalPosition.Y > 1.0f || LocalPosition.X<0.0f || LocalPosition.Y<0.0f) return false;
	
	LocalPosition.X *= ImageWidth-1;
	LocalPosition.Y *= ImageHeight-1;
	const int BufferPosition = ((int)LocalPosition.Y * ImageWidth) + (int)LocalPosition.X;
	
	if (TextureData[BufferPosition] <= AdvancedHitAlpha) return false;
	else return true;
}

FReply SAlphaButton::OnMouseMove(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	if (TextureData.IsEmpty())
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