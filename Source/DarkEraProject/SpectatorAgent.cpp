#include "SpectatorAgent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

ASpectatorAgent::ASpectatorAgent()
{
    PrimaryActorTick.bCanEverTick = true;
    RotationSpeed = 0.001f;
    ZoomSpeed = 20.0f;
    MinZoomDistance = 100.0f;
    MaxZoomDistance = 500.0f;
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
}

void ASpectatorAgent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    FVector CurrentPosition = GetViewTarget()->GetActorLocation();
    FVector DirectionToTarget = TargetPosition - CurrentPosition;
    DirectionToTarget.Normalize();

    FRotator NewRotation = FRotationMatrix::MakeFromX(DirectionToTarget).Rotator();
    GetViewTarget()->SetActorRotation(NewRotation);

    FVector NewPosition = TargetPosition - (DirectionToTarget * MaxZoomDistance);
    GetViewTarget()->SetActorLocation(NewPosition);
    
}

void ASpectatorAgent::RotateCamera()
{
    if (bIsDragging)
    {
        if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
        {
            FVector2D CurrentMousePosition;
            PlayerController->GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);

            FVector2D DeltaMousePosition = CurrentMousePosition - InitialMousePosition;

            FRotator CurrentRotation = GetViewTarget()->GetActorRotation();
            CurrentRotation.Yaw += DeltaMousePosition.X * RotationSpeed;
            CurrentRotation.Pitch += DeltaMousePosition.Y * RotationSpeed;

            GetViewTarget()->SetActorRotation(CurrentRotation);

            InitialMousePosition = CurrentMousePosition;
        }
    }
}

void ASpectatorAgent::ZoomIn()
{
    ZoomCamera(1.0f);
}

void ASpectatorAgent::ZoomOut()
{
    ZoomCamera(-1.0f);
}

void ASpectatorAgent::MoveCameraUp()
{
    if(GetViewTarget() != nullptr)
    {
        FVector CurrentPosition = GetViewTarget()->GetActorLocation();
        CurrentPosition += GetViewTarget()->GetActorUpVector() * ZoomSpeed;
        GetViewTarget()->SetActorLocation(CurrentPosition);
    }
}

void ASpectatorAgent::MoveCameraDown()
{
    if(GetViewTarget() != nullptr)
    {
        FVector CurrentPosition = GetViewTarget()->GetActorLocation();
        CurrentPosition -= GetViewTarget()->GetActorUpVector() * ZoomSpeed;
        GetViewTarget()->SetActorLocation(CurrentPosition);
    }
}

void ASpectatorAgent::MoveCameraLeft()
{
    if(GetViewTarget() != nullptr)
    {
        FVector CurrentPosition = GetViewTarget()->GetActorLocation();
        CurrentPosition -= GetViewTarget()->GetActorRightVector() * ZoomSpeed;
        GetViewTarget()->SetActorLocation(CurrentPosition);
    }
}

void ASpectatorAgent::MoveCameraRight()
{
    if(GetViewTarget() != nullptr)
    {
        FVector CurrentPosition = GetViewTarget()->GetActorLocation();
        CurrentPosition += GetViewTarget()->GetActorRightVector() * ZoomSpeed;
        GetViewTarget()->SetActorLocation(CurrentPosition);
        UE_LOG(LogTemp, Warning, TEXT("Right"));
    }
    UE_LOG(LogTemp, Warning, TEXT("Right"));
}

AActor* ASpectatorAgent::GetViewTarget()
{
    return CameraComponent->GetOwner();
}

void ASpectatorAgent::ZoomCamera(float DeltaZoom)
{
    if(GetViewTarget() != nullptr)
    {
        FVector CurrentPosition = GetViewTarget()->GetActorLocation();
        FVector DirectionToTarget = TargetPosition - CurrentPosition;
        float CurrentDistance = DirectionToTarget.Size();

        float NewDistance = FMath::Clamp(CurrentDistance + DeltaZoom * ZoomSpeed, MinZoomDistance, MaxZoomDistance);

        DirectionToTarget.Normalize();
        FVector NewPosition = TargetPosition - (DirectionToTarget * NewDistance);

        GetViewTarget()->SetActorLocation(NewPosition);
    }
}
