// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define BOOL_STR(b) (TEXT("False\0True")+b*6)

namespace UHelpers {

	bool Trace(UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& dir, float length, FHitResult& HitResult, ECollisionChannel CollisionChannel = ECC_Pawn, bool ReturnPhysMat = false);
	bool Trace(UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& end, FHitResult& HitResult, ECollisionChannel CollisionChannel = ECC_Pawn, bool ReturnPhysMat = false);

	bool TraceMulti(UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& dir, float length, TArray<FHitResult>& HitResults, ECollisionChannel CollisionChannel = ECC_Pawn, bool ReturnPhysMat = false);
	bool TraceMulti(UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& end, TArray<FHitResult>& HitResults, ECollisionChannel CollisionChannel = ECC_Pawn, bool ReturnPhysMat = false);


	bool DbgTrace(bool renderTrace, UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& dir, float length, FHitResult& HitResult, ECollisionChannel CollisionChannel = ECC_Pawn, bool ReturnPhysMat = false);
	bool DbgTrace(bool renderTrace, UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& end, FHitResult& HitResult, ECollisionChannel CollisionChannel = ECC_Pawn, bool ReturnPhysMat = false);

	bool DbgTraceMulti(bool renderTrace, UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& dir, float length, TArray<FHitResult>& HitResults, ECollisionChannel CollisionChannel = ECC_Pawn, bool ReturnPhysMat = false);
	bool DbgTraceMulti(bool renderTrace, UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& end, TArray<FHitResult>& HitResults, ECollisionChannel CollisionChannel = ECC_Pawn, bool ReturnPhysMat = false);

}