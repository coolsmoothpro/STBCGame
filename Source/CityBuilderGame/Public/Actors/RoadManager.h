// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoadManager.generated.h"

UCLASS()
class CITYBUILDERGAME_API ARoadManager : public AActor
{
	GENERATED_BODY()
protected:
	
	ARoadManager();

	UPROPERTY(BlueprintReadWrite, EditAnywhere , Category = "Road | Meshes")
	class UStaticMesh* roadmesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Road | freeRoads")
	TArray<UFreeRoadGeneratorComponent*> freeRoads;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	class UFreeRoadGeneratorComponent* AddNewFreeRoad();

};
