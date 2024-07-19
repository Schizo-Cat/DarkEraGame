// Fill out your copyright notice in the Description page of Project Settings.

#include "AlphaOverlapButton.h"

#include "Components/ButtonSlot.h"
#include "Components/CanvasPanel.h"

#pragma region >>> SAlphaOverlapButton <<<

void SAlphaOverlapButton::SetTextureData(UTexture2D* InTexture)
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

FReply SAlphaOverlapButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

FReply SAlphaOverlapButton::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) 
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

FReply SAlphaOverlapButton::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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
	return SButton::OnMouseButtonUp(MyGeometry, MouseEvent);
}

void SAlphaOverlapButton::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

void SAlphaOverlapButton::OnMouseLeave(const FPointerEvent& MouseEvent) 
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
		CachedOverlap = false;
	}
	return SButton::OnMouseLeave(MouseEvent);
}

bool SAlphaOverlapButton::IsMouseOverOpaquePixel( const FPointerEvent & MouseEvent) const
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

FReply SAlphaOverlapButton::OnMouseMove(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	if (TextureData.IsEmpty()) 
		return SButton::OnMouseMove(MyGeometry, MouseEvent);

	TArray<SAlphaOverlapButton*> AllButtons;
	AllButtons.Add(this);
	for (auto Btn : OverlapButtons)
	{
		if (Btn.IsValid())
		{
			AllButtons.Add(Btn.Get());
		}
	}

	SAlphaOverlapButton* PreviousOverlap = nullptr;
	SAlphaOverlapButton* CurrentOverlap = nullptr;
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


FCursorReply SAlphaOverlapButton::OnCursorQuery(const FGeometry & MyGeometry, const FPointerEvent & CursorEvent) const
{
	if (!IsHovered()) return FCursorReply::Unhandled();
	TOptional<EMouseCursor::Type> TheCursor = GetCursor();
	return (TheCursor.IsSet()) ? FCursorReply::Cursor(TheCursor.GetValue()) : FCursorReply::Unhandled();
}

#pragma endregion

#pragma region >>> UAlphaOverlapButton <<<

UAlphaOverlapButton::UAlphaOverlapButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  AdvancedHitTexture(nullptr),
	  AdvancedHitAlpha(0), SiblingIndex(0)
{
	
}

void UAlphaOverlapButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	
	if(MyButton.IsValid())
	{ 
		if(AdvancedHitTexture) (static_cast<SAlphaOverlapButton*>(MyButton.Get()))->SetTextureData(AdvancedHitTexture);
		(static_cast<SAlphaOverlapButton*>(MyButton.Get()))->SetAdvancedHitAlpha(AdvancedHitAlpha);
		TArray<TSharedPtr<SAlphaOverlapButton>> OverlapButtonsTemp;
	
		for (const auto Btn : OverlapButtons)
		{
			if (  IsValid(Btn) && Btn->GetCacheSButton().IsValid()) 
			{
				OverlapButtonsTemp.Add(StaticCastSharedPtr<SAlphaOverlapButton>(Btn->GetCacheSButton()));
			}
		}
	 
		(static_cast<SAlphaOverlapButton*>(MyButton.Get()))->OverlapButtons = OverlapButtonsTemp;


		
		bool ParentFinded = false;
		UWidget* ParentWidget = GetParent();
		while (!ParentFinded)
		{
			if(const UPanelWidget* Parent = Cast<UPanelWidget>(ParentWidget))
			{
				SiblingIndex= Parent->GetChildIndex(this);
				(static_cast<SAlphaOverlapButton*>(MyButton.Get()))->SiblingIndex = SiblingIndex;
				ParentFinded=true;
			}
			else
			{
				//ParentWidget = ParentWidget->GetParent();
				if(!ParentWidget)
				{
					ParentFinded=true;
				}
			}
		}
	}
}


TSharedRef<SWidget> UAlphaOverlapButton::RebuildWidget()
{
	const TSharedPtr<SAlphaOverlapButton> AlphaButton = SNew(SAlphaOverlapButton)
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