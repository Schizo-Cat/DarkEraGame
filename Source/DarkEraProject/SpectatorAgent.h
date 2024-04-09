#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "SpectatorAgent.generated.h"

UCLASS()
class DARKERAPROJECT_API ASpectatorAgent : public APawn
{
	GENERATED_BODY()

public:
	ASpectatorAgent();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parameters")
	FVector TargetPosition;

	UFUNCTION(BlueprintCallable)
	void RotateCamera();
	UFUNCTION(BlueprintCallable)
	void ZoomIn();
	UFUNCTION(BlueprintCallable)
	void ZoomOut();
	UFUNCTION(BlueprintCallable)
	void MoveCameraUp();
	UFUNCTION(BlueprintCallable)
	void MoveCameraDown();
	UFUNCTION(BlueprintCallable)
	void MoveCameraLeft();
	UFUNCTION(BlueprintCallable)
	void MoveCameraRight();

private:
	float RotationSpeed;
	float ZoomSpeed;
	float MinZoomDistance;
	float MaxZoomDistance;

	AActor* CameraActor;

	FVector2D InitialMousePosition;
	bool bIsDragging;
	
	AActor* GetViewTarget();
	void ZoomCamera(float DeltaZoom);
};