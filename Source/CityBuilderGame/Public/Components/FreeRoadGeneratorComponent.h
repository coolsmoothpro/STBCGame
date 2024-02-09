// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "FreeRoadGeneratorComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CITYBUILDERGAME_API UFreeRoadGeneratorComponent : public USplineComponent
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Road | Meshes")
	class UStaticMesh* roadmesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Road | dimentions")
	float RoadMeshLength = 500;

	bool bShouldUpdate = true;

	UFreeRoadGeneratorComponent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//virtual void OnConstruction(const FTransform& transform) override;
	
	UFUNCTION(BlueprintCallable)
	void UpdateRoadMesh();

	/*
	
	*/
	UFUNCTION(BlueprintCallable)
	void OnStart(FVector start_loc);
	UFUNCTION(BlueprintCallable)
	void OnUpdate(FVector update_loc, FVector direction, bool bAutoDirection = true);
	UFUNCTION(BlueprintCallable)
	void AddPointAtLoc(FVector loc);
	UFUNCTION(BlueprintCallable)
	void OnStop(FVector end_loc);

	/*
		scales the tangent on spline point !
		@param pointIndex is the index of the point
		@param scaler     is a 0.0 - 1.0 value to scale the tanget (from NAN to full tangent)
	*/
	UFUNCTION(BlueprintCallable)
	void UpdatePointTangentScale(int32 pointIndex, float scaler);


	int32 GetLastSplineIndex()const;


public:

	void SetRoadMesh(UStaticMesh* inMesh);

	
};
