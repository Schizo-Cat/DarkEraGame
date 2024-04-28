#include "SpectatorAgent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/SpringArmComponent.h"

ASpectatorAgent::ASpectatorAgent()
{
    PrimaryActorTick.bCanEverTick = true;
    RotationSpeed = 10.0f;
    ZoomSpeed = 20.0f;
    
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    //CameraBoom->TargetArmLength = 0;
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
    
    UE_LOG(LogTemp, Warning, TEXT("Mass X: %s"), *GetViewTarget()->GetActorRotation().ToString());
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
        if(GetViewTarget()->GetActorRotation().Pitch > -86)
            CurrentPosition += GetViewTarget()->GetActorUpVector() * RotationSpeed;
        GetViewTarget()->SetActorLocation(CurrentPosition);
    }
}

void ASpectatorAgent::MoveCameraDown()
{
    if(GetViewTarget() != nullptr)
    {
        FVector CurrentPosition = GetViewTarget()->GetActorLocation();
        if(GetViewTarget()->GetActorRotation().Pitch < 86)
            CurrentPosition -= GetViewTarget()->GetActorUpVector() * RotationSpeed;
        GetViewTarget()->SetActorLocation(CurrentPosition);
    }
}

void ASpectatorAgent::MoveCameraLeft()
{
    if(GetViewTarget() != nullptr)
    {
        FVector CurrentPosition = GetViewTarget()->GetActorLocation();
        CurrentPosition -= GetViewTarget()->GetActorRightVector() * RotationSpeed * GetViewTarget()->GetActorUpVector().Z;
        GetViewTarget()->SetActorLocation(CurrentPosition);
    }
}

void ASpectatorAgent::MoveCameraRight()
{
    if(GetViewTarget() != nullptr)
    {
        FVector CurrentPosition = GetViewTarget()->GetActorLocation();
        CurrentPosition += GetViewTarget()->GetActorRightVector() * RotationSpeed * GetViewTarget()->GetActorUpVector().Z;
        GetViewTarget()->SetActorLocation(CurrentPosition);
    }
}

AActor* ASpectatorAgent::GetViewTarget()
{
    return CameraComponent->GetOwner();
}

void ASpectatorAgent::ZoomCamera(float DeltaZoom)
{
    if(GetViewTarget() != nullptr)
    {
        //FVector NewOffset = FVector(DeltaZoom, 0.0f, 0.0f) + CameraBoom->SocketOffset;
        float CurrentDistance = CameraBoom->TargetArmLength;
        float NewDistance = FMath::Clamp(CurrentDistance + DeltaZoom * ZoomSpeed, MinZoomDistance, MaxZoomDistance);
        CameraBoom->TargetArmLength = NewDistance;
        // FVector CurrentPosition = GetViewTarget()->GetActorLocation();
        // FVector DirectionToTarget = TargetPosition - CurrentPosition;
        // float CurrentDistance = DirectionToTarget.Size();
        //
        // float NewDistance = FMath::Clamp(CurrentDistance + DeltaZoom * ZoomSpeed, MinZoomDistance, MaxZoomDistance);
        //
        // DirectionToTarget.Normalize();
        // FVector NewPosition = TargetPosition - (DirectionToTarget * NewDistance);
        //
        // GetViewTarget()->SetActorLocation(NewPosition);
    }
}
