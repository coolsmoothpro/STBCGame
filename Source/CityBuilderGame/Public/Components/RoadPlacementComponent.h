// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/CBGTypes.h"
#include "RoadPlacementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CITYBUILDERGAME_API URoadPlacementComponent : public UActorComponent
{
	GENERATED_BODY()

	// Internal
	bool bRoadPlacementStarted = false;
	struct FGridCell _roadplacement_StartCell;
	struct FGridCell _roadplacement_CurrentCell;


protected:
	// Sets default values for this component's properties
	URoadPlacementComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// returns player controller of this player
	class ACBGPlayerController* GetPlayerController()const;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable , Category = "Road Placement")
	void RoadPlacement_Start();
	// can be called on tick, updates the current location of the cursor during the road placment process
	void RoadPlacement_Update();
	UFUNCTION(BlueprintCallable, Category = "Road Placement")
	void RoadPlacement_Accept();
	UFUNCTION(BlueprintCallable, Category = "Road Placement")
	void RoadPlacement_Cancel();



	TArray<struct FGridCell> CalculateSelectedRoads(bool bCalculateInAnyCase = false);


	// only for debug !
	UFUNCTION(BlueprintCallable, Category = "Road Placement")
	void PrintCell();
};
