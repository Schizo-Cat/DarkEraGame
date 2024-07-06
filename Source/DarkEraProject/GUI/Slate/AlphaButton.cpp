#include "AlphaButton.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UAlphaButton::NativeConstruct()
{
    Super::NativeConstruct();

    if (ButtonImage)
    {
        // Use the getter method for the brush texture
        ButtonTexture = Cast<UTexture2D>(ButtonImage->GetBrush().GetResourceObject());
    }
}

void UAlphaButton::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    FVector2D LocalMousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

    if (IsOpaquePixel(LocalMousePosition.X, LocalMousePosition.Y))
    {
        OnMouseHovered();
    }
    else
    {
        OnMouseUnhovered();
    }
}

void UAlphaButton::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    OnMouseUnhovered();
}

FReply UAlphaButton::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FVector2D LocalMousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
    if (ButtonTexture && IsOpaquePixel(LocalMousePosition.X, LocalMousePosition.Y))
    {
        OnButtonClicked();
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

bool UAlphaButton::IsOpaquePixel(int32 X, int32 Y)
{
    if (!ButtonTexture) return false;

    FTexturePlatformData* PlatformData = ButtonTexture->GetPlatformData();
    if (!PlatformData) return false;

    const FColor* FormData = static_cast<const FColor*>(PlatformData->Mips[0].BulkData.Lock(LOCK_READ_ONLY));
    if (!FormData) return false;

    FColor PixelColor = FormData[Y * PlatformData->SizeX + X];
    PlatformData->Mips[0].BulkData.Unlock();

    return PixelColor.A > 0;
}

void UAlphaButton::OnMouseHovered()
{
    // Handle the hover effect here
    // For example, change the button color or show an outline
    if (ButtonImage)
    {
        // Example hover effect
        ButtonImage->SetOpacity(0.8f);
    }
}

void UAlphaButton::OnMouseUnhovered()
{
    // Handle the unhover effect here
    // For example, revert the button color or hide the outline
    if (ButtonImage)
    {
        // Example unhover effect
        ButtonImage->SetOpacity(1.0f);
    }
}

void UAlphaButton::OnButtonClicked()
{
    // Handle the button click event
    // This can be customized as needed
    UE_LOG(LogTemp, Warning, TEXT("Button clicked!"));
}