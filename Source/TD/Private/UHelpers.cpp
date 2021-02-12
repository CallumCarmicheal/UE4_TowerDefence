// Fill out your copyright notice in the Description page of Project Settings.

#include "UHelpers.h"
#include "DrawDebugHelpers.h"

#define TD_TRACE_DBG_TAG TEXT("TD Trace")

bool UHelpers::Trace(UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& dir, float length, 
	FHitResult& HitResult, ECollisionChannel collisionChannel, bool returnPhysMat)
{
	return DbgTrace(false, world, actorToIgnore, start, dir, length, HitResult, collisionChannel, returnPhysMat);
}

//Trace using start point, direction, and length
bool UHelpers::DbgTrace(bool renderTrace, UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& dir, float length, 
	FHitResult& HitResult, ECollisionChannel collisionChannel, bool returnPhysMat)
{
	return DbgTrace(renderTrace, world, actorToIgnore, start, start + dir * length, HitResult, collisionChannel, returnPhysMat);
}

bool UHelpers::Trace(UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& end, 
	FHitResult& HitResult, ECollisionChannel collisionChannel, bool returnPhysMat)
{
	return DbgTrace(false, world, actorToIgnore, start, end, HitResult, collisionChannel, returnPhysMat);
}

//Trace using start point, and end point
bool UHelpers::DbgTrace(bool renderTrace, UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& end, 
	FHitResult& HitResult, ECollisionChannel collisionChannel, bool returnPhysMat)
{
	if (!world) 
		return false;

	//Trace params, set the 'false' to 'true' if you want it to trace against the actual meshes instead of their collision boxes.
	FCollisionQueryParams TraceParams(FName(TD_TRACE_DBG_TAG), false, actorToIgnore);
	TraceParams.bReturnPhysicalMaterial = returnPhysMat;

	//Ignore Actors, usually the actor that is calling the trace
	TraceParams.AddIgnoredActor(actorToIgnore);

	//Re-initialize hit info, so you can call the function repeatedly and hit will always be fresh
	HitResult = FHitResult(ForceInit);
	
	//Trace!
	const bool bHitSomething = world->LineTraceSingleByChannel(
		HitResult,			//result
		start,				//start
		end,				//end
		collisionChannel,	//collision channel
		TraceParams
	);

	if (renderTrace) {
		// Draw a square at the impact point.
		if (bHitSomething)
			DrawDebugSolidBox(world, HitResult.ImpactPoint, FVector(10, 10, 10), FQuat(0, 0, 0, 0), FColor::Yellow, false, 1.0f / 60.0f);

		// Draw the trace line. Red if something was hit, green if nothing was hit.
		DrawDebugLine(world, start, end, (bHitSomething ? FColor(255, 0, 0) : FColor(0, 255, 0)), false, -1, 0, 4);
	}

	return bHitSomething;
}

bool UHelpers::TraceMulti(UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& dir, float length,
	TArray<FHitResult>& HitResults, ECollisionChannel CollisionChannel, bool ReturnPhysMat)
{
	return DbgTraceMulti(false, world, actorToIgnore, start, start + dir * length, HitResults, CollisionChannel, ReturnPhysMat);
}

bool UHelpers::DbgTraceMulti(bool renderTrace, UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& dir, float length,
	TArray<FHitResult>& HitResults, ECollisionChannel CollisionChannel, bool ReturnPhysMat)
{
	return DbgTraceMulti(renderTrace, world, actorToIgnore, start, start + dir * length, HitResults, CollisionChannel, ReturnPhysMat);
}

bool UHelpers::TraceMulti(UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& end, 
	TArray<FHitResult>& HitResults, ECollisionChannel collisionChannel, bool returnPhysMat)
{
	return DbgTraceMulti(false, world, actorToIgnore, start, end, HitResults, collisionChannel, returnPhysMat);
}

bool UHelpers::DbgTraceMulti(bool renderTrace, UWorld* world, AActor* actorToIgnore, const FVector& start, const FVector& end, 
	TArray<FHitResult>& HitResults, ECollisionChannel collisionChannel, bool returnPhysMat)
{
	if (!world) 
		return false;

	//Trace params, set the 'false' to 'true' if you want it to trace against the actual meshes instead of their collision boxes.
	FCollisionQueryParams TraceParams(FName(TD_TRACE_DBG_TAG), false, actorToIgnore);
	TraceParams.bReturnPhysicalMaterial = returnPhysMat;

	//Ignore Actors, usually the actor that is calling the trace
	TraceParams.AddIgnoredActor(actorToIgnore);

	//Re-initialize hit info, so you can call the function repeatedly and hit will always be fresh
	HitResults.Empty();

	//Trace!
	const bool bHitSomething = world->LineTraceMultiByChannel(
		HitResults,	      //result
		start,			  //start
		end,			  //end
		collisionChannel, //collision channel
		TraceParams
	);

	if (renderTrace) {
		// Draw a square at the impact point.
		if (bHitSomething) {
			for (auto hit : HitResults)
				//DrawDebugPoint(world, hit.ImpactPoint, 100, FColor(255, 255, 0), false, -1);
				DrawDebugSolidBox(world, hit.ImpactPoint, FVector(10, 10, 10), FQuat(0, 0, 0, 0), FColor::Yellow, false, 1.0f / 60.0f);
		}

		// Draw the trace line. Red if something was hit, green if nothing was hit.
		DrawDebugLine(world, start, end, (bHitSomething ? FColor(255, 0, 0) : FColor(0, 255, 0)), false, -1, 0, 4);
	}

	return bHitSomething;
}