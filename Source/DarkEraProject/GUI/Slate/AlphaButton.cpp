// Fill out your copyright notice in the Description page of Project Settings.

#include "AlphaButton.h"

#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"

#pragma region >>> SAlphaButton <<<

void SAlphaButton::SetTextureData(UTexture2D* InTexture)
{
	if (InTexture) {
		TextureData = static_cast<FColor*>((InTexture->GetPlatformData()->Mips[0]).BulkData.Lock(LOCK_READ_ONLY));
		ImageHeight = InTexture->GetPlatformData()->SizeY;
		ImageWidth = InTexture->GetPlatformData()->SizeX;
		InTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	}
}

FReply SAlphaButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (TextureData)
	{
		if (!CachedOverlap) 
		{
			for (auto Btn : OverlapButtons)
			{
				if (Btn.IsValid() && Btn->CachedOverlap)
				{
					return Btn->SButton::OnMouseButtonDown(Btn->GetCachedGeometry(), MouseEvent);
				}
			}
			return FReply::Unhandled();
		}
		return SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
	}
	return SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply SAlphaButton::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) 
{
	if (TextureData)
	{
		if (!CachedOverlap)
		{
			for (auto Btn : OverlapButtons)
			{
				if (Btn.IsValid() && Btn->CachedOverlap)
				{
					return Btn->SButton::OnMouseButtonDoubleClick(Btn->GetCachedGeometry(), InMouseEvent);
				}
			}
			return FReply::Unhandled();
		}
		return SButton::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	}
	return SButton::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FReply SAlphaButton::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (TextureData)
	{
		if (!CachedOverlap)
		{
			for (auto Btn : OverlapButtons)
			{
				if (Btn.IsValid() && Btn->CachedOverlap || Btn->IsPressed())
				{
					Btn->SButton::OnMouseButtonUp(Btn->GetCachedGeometry(), MouseEvent);
				}
			}
		}
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
		if (!CachedOverlap)
		{
			for (auto Btn : OverlapButtons)
			{
				if (Btn.IsValid() && Btn->CachedOverlap)
				{
					if(!Btn->IsMouseOverOpaquePixel(MouseEvent))
					{
						Btn->CachedOverlap = false;
						if (Btn->IsHovered())
						{
							Btn->OnMouseLeave(MouseEvent);
						}
					}
				}
			}
		}
		else
		{
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

	TArray<SAlphaButton *> AllButtons;
	AllButtons.Add(this);
	for (auto Btn : OverlapButtons)
	{
		if (Btn.IsValid())
		{
			AllButtons.Add(Btn.Get());
		}
	}

	SAlphaButton *PreviousOverlap = nullptr;
	SAlphaButton *CurrentOverlap = nullptr;
	for (const auto Btn : AllButtons)
	{
		if (!PreviousOverlap && Btn->CachedOverlap)
		{
			PreviousOverlap = Btn;
		}
		if(Btn->IsMouseOverOpaquePixel(MouseEvent))
		{
			if ( !CurrentOverlap || (CurrentOverlap && (Btn->SiblingIndex > CurrentOverlap->SiblingIndex) ) )
			{
				CurrentOverlap = Btn;
			}
		}
	}
	
	if(PreviousOverlap == CurrentOverlap && CurrentOverlap == this)
	{
		return SButton::OnMouseMove(MyGeometry, MouseEvent);
	}
	else if(PreviousOverlap != CurrentOverlap)
	{
		if (PreviousOverlap) 
		{
			PreviousOverlap->CachedOverlap = false;
			if (PreviousOverlap->IsHovered()) 
			{
				PreviousOverlap->OnMouseLeave(MouseEvent);
			}
		}
		if (CurrentOverlap)
		{
			CurrentOverlap->CachedOverlap = true;
			if (!CurrentOverlap->IsHovered())
			{
				CurrentOverlap->OnMouseEnter(CurrentOverlap->GetCachedGeometry(), MouseEvent);
			}
		}
	}
	else
	{
		if (CurrentOverlap == nullptr)
		{
			if (IsHovered())
			{
				CachedOverlap = false;
				OnMouseLeave(MouseEvent);
			}
		}
		else
		{
			CurrentOverlap->SButton::OnMouseMove(CurrentOverlap->GetCachedGeometry(), MouseEvent);
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
	  AdvancedHitAlpha(0), SiblingIndex(0)
{
}

void UAlphaButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	if(MyButton.IsValid())
	{ 
		(static_cast<SAlphaButton*>(MyButton.Get()))->SetTextureData(AdvancedHitTexture);
		(static_cast<SAlphaButton*>(MyButton.Get()))->SetAdvancedHitAlpha(AdvancedHitAlpha);
		TArray<TSharedPtr<SAlphaButton>> OverlapButtonsTemp;
	
		for (const auto Btn : OverlapButtons)
		{
			if (  IsValid(Btn) && Btn->GetCacheSButton().IsValid()) 
			{
				OverlapButtonsTemp.Add(StaticCastSharedPtr<SAlphaButton>(Btn->GetCacheSButton()));
			}
		}
	 
		(static_cast<SAlphaButton*>(MyButton.Get()))->OverlapButtons= OverlapButtonsTemp;


		if (const UCanvasPanel* Parent = CastChecked<UCanvasPanel>(GetParent()); IsValid(Parent)) 
		{
			SiblingIndex= Parent->GetChildIndex(this);
			(static_cast<SAlphaButton*>(MyButton.Get()))->SiblingIndex = SiblingIndex;
		}
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

	AlphaButton->SetTextureData(AdvancedHitTexture);
	AlphaButton->SetAdvancedHitAlpha(AdvancedHitAlpha);

	MyButton = AlphaButton;

	if (GetChildrenCount() > 0) Cast<UButtonSlot>(GetContentSlot())->BuildSlot(MyButton.ToSharedRef());

	return MyButton.ToSharedRef();
}
#pragma endregion