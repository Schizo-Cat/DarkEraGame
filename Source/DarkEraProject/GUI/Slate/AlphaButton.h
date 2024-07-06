#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AlphaButton.generated.h"

class UTexture2D;
class UImage;

UCLASS()
class DARKERAPROJECT_API UAlphaButton : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
    UPROPERTY(meta = (BindWidget))
    UImage* ButtonImage;

    UTexture2D* ButtonTexture;

    bool IsOpaquePixel(int32 X, int32 Y);

    UFUNCTION()
    void OnMouseHovered();

    UFUNCTION()
    void OnMouseUnhovered();

    UFUNCTION()
    void OnButtonClicked();
};