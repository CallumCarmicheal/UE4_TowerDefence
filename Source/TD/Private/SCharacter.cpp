// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "UHelpers.h"

#define UI_DBG_PRINT(COLOR, TXT, ...) \
	GEngine->AddOnScreenDebugMessage(-1, 1 / 60, COLOR, FString::Printf(TEXT( TXT ), __VA_ARGS__));

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Controls = CreateDefaultSubobject<USCharacterControls>(TEXT("Controls"));
	
	// Add components
	MovementMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MovementMeshComp")); // TODO: Check if this is the best component for this?
	MovementMeshComp->SetVisibility(false, false);
	RootComponent = MovementMeshComp;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetRelativeRotation(FRotator(-60, 0, 0));
	SpringArmComp->TargetArmLength = 500.0f;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleCameraMovement(DeltaTime);
	MouseCursorLogic();
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Movement_Forward", this, &ASCharacter::InputMovementForward);
	PlayerInputComponent->BindAxis("Movement_Right", this, &ASCharacter::InputMovementRight);
	PlayerInputComponent->BindAxis("Camera_Zoom", this, &ASCharacter::InputCameraZoom);
	PlayerInputComponent->BindAxis("Camera_Rotation", this, &ASCharacter::InputCameraRotation);


	DECLARE_DELEGATE_OneParam(FInputKeyHeld, const bool);
	PlayerInputComponent->BindAction<FInputKeyHeld>("Camera_RotateMouseShow", IE_Pressed,  this, &ASCharacter::InputCameraRotate_MouseVisibility, true);
	PlayerInputComponent->BindAction<FInputKeyHeld>("Camera_RotateMouseShow", IE_Released, this, &ASCharacter::InputCameraRotate_MouseVisibility, false);
}

void ASCharacter::InputMovementForward(float Value) {
	this->m_Controls.Forward = Value;
}

void ASCharacter::InputMovementRight(float Value) {
	this->m_Controls.Right = Value;
}

void ASCharacter::InputCameraZoom(float V) {
	check(Controls != nullptr);

	const float ChangeAmount = Controls->CameraZoomChangeAmount * V;

	m_Controls.TargetZoom = FMath::Clamp(
		ChangeAmount + m_Controls.TargetZoom,
		Controls->CameraZoomMin, Controls->CameraZoomMax
	);

	UI_DBG_PRINT(FColor::Yellow, "V = %f, Target Zoom = %f, Current Zoom = %f", V, m_Controls.TargetZoom, SpringArmComp->TargetArmLength);
}

void ASCharacter::InputCameraRotation(float Value) {
	m_Controls.Rotation = Value;
}

void ASCharacter::InputCameraRotate_MouseVisibility(bool isPressed) {
	m_Controls.MouseCameraRotateHeld = isPressed;
}

void ASCharacter::HandleCameraMovement(float ΔT) {
	// TODO: Figure out how networking would work for this. Tomorrows problem not today!
	const APlayerController* PlayerController = Cast<APlayerController>(Controller);

	// Handle Camera Translation
	const float CameraSpeedModifier		= FMath::Clamp(FMath::Abs(m_Controls.TargetZoom / 500), 0.6f, 3.6f);
	const float CameraMovementSpeed		= Controls->CameraMovementSpeed * CameraSpeedModifier;
	const FVector MovementForwardVector = (MovementMeshComp->GetForwardVector() * m_Controls.Forward) * CameraMovementSpeed;
	const FVector MovementRightVector   = (MovementMeshComp->GetRightVector() * m_Controls.Right) * CameraMovementSpeed;
	const FVector UpVector				= MovementMeshComp->GetUpVector();
	const FVector MotionVector			= (MovementForwardVector + MovementRightVector) * ΔT;

	FVector boundsMin, boundsMax;
	MovementMeshComp->GetLocalBounds(boundsMin, boundsMax);
	
	FVector WorldLocation = MovementMeshComp->GetComponentLocation() + MotionVector;

	// Handle Camera Zoom
	SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, m_Controls.TargetZoom, ΔT, 10.0f);

	// Handle camera height adjustment.
	FVector CameraLocation = WorldLocation;
	CameraLocation.Z = 0;
		
	// Get 3000 units above the character and 1000 below it, This will
	// trace from top to bottom and allow us to get the top most object in the scene
	FVector TraceStart = CameraLocation + (UpVector * 3000);
	FVector TraceEnd   = CameraLocation - (UpVector * 1000);

	FHitResult HitResult;
	bool hit = UHelpers::Trace(GetWorld(), this, TraceStart, TraceEnd, HitResult, ECC_Visibility);

	// This will smooth out the camera translation.
	FVector TargetWorldLocation = FMath::VInterpTo(GetActorLocation(), WorldLocation, ΔT, 100.0f);
	
	if (hit) {
		FString locationString = HitResult.Location.ToCompactString();
		UI_DBG_PRINT(FColor::Orange, "Camera Trace { Loc = %s }", *locationString);

		FVector LocationPadding = UpVector * 50;
		LocationPadding.Z += boundsMax.Z;

		// This will smooth out the camera vertical movement
		SetActorLocation(FMath::VInterpTo(TargetWorldLocation, HitResult.ImpactPoint + LocationPadding, ΔT, 5.0f));
	} else {
		UI_DBG_PRINT(FColor::Red, "Camera Error: Trace did not hit any geometry!!!");

		// Update the actor location without any modification on the Z axis.
		SetActorLocation(TargetWorldLocation);
	}

	// Handle Camera Rotation
	if (PlayerController && !PlayerController->ShouldShowMouseCursor()) {
		// TODO: Smooth out user rotation.
		
		FRotator ActorRotation = GetActorRotation();
		FRotator TargetRotation = ActorRotation.Add(0, (m_Controls.Rotation * ΔT) * Controls->CameraRotationSpeed, 0);
		SetActorRotation(TargetRotation);
		//SetActorRotation(FMath::RInterpTo(ActorRotation, TargetRotation, ΔT, 10.0f));

		UI_DBG_PRINT(FColor::Red, "Camera Rotation Value: { %f }", m_Controls.Rotation);
	}
}

void ASCharacter::MouseCursorLogic() {
	// TODO: Fix issue where mouse does not loop to other side of window!

	// const bool ShowCursor = !m_Controls.MouseCameraRotateHeld;
	// 
	// APlayerController* PlayerController = Cast<APlayerController>(Controller);
	// if (PlayerController) {
	// 	PlayerController->SetShowMouseCursor(ShowCursor);
	// } 
}
