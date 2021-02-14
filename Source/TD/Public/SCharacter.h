// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SCharacterControls.h"
#include "SCharacter.generated.h"

struct FCharacterControls;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TD_API ASCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected: /// Controls

	
private:
	struct {
		float Forward = 0.0f;
		float Right = 0.0f;
		float RotationYaw = 0.0f;
		float RotationPitch = 0.0f;

		float TargetZoom = 500.0f;
		bool MouseCameraRotateHeld = false;
	} m_Controls;

	void InputMovementForward(float Value);
	void InputMovementRight(float Value);
	void InputCameraZoom(float Value);
	void InputCameraYaw(float Value);
	void InputCameraPitch(float Value);
	void InputCameraRotate_MouseVisibility(bool isPressed);

	void HandleCameraMovement(float DeltaTime);
	void MouseCursorLogic() const;
	
protected: /// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MovementMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USCharacterControls* Controls;
};


//
//
//USTRUCT(BlueprintType)
//struct TD_API FControlsSettings {
//
//	GENERATED_USTRUCT_BODY()
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float ZoomMin = 100.0f;
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float ZoomDefault = 500.0f;
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) float ZoomMax= 500.0f;
//	
//	FControlsSettings() {
//		//
//	}
//};
//
//
//USTRUCT(BlueprintType)
//struct TD_API FCharacterControls {
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
//	FControlsSettings Settings;
//
//	float MovementForward = 0.0f;
//	float MovementRight = 0.0f;
//	
//	FCharacterControls() {
//		//
//	}
//};