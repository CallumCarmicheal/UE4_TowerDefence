// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCharacterControls.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (TD), meta = (BlueprintSpawnableComponent))
class TD_API USCharacterControls : public UActorComponent
{
	GENERATED_BODY()
	
public:
	USCharacterControls();
	~USCharacterControls();

	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	float CameraZoomMin = 200.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	float CameraZoomDefault = 500.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	float CameraZoomMax = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float CameraMovementSpeed = 500.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float CameraRotationSpeed = 30.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float CameraZoomChangeAmount = 10.0;
};
