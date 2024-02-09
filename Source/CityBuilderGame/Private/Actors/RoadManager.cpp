// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RoadManager.h"
#include "Components/splinecomponent.h"
#include "Components/splineMeshcomponent.h"
#include "Components/FreeRoadGeneratorComponent.h"
#include "CityBuilderGame/CityBuilderGame.h"


// Sets default values
ARoadManager::ARoadManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(FName("root"));
	SetRootComponent(root);

}

// Called when the game starts or when spawned
void ARoadManager::BeginPlay()
{
	Super::BeginPlay();


}



float tickcounter = 0;
// Called every frame
void ARoadManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UFreeRoadGeneratorComponent* ARoadManager::AddNewFreeRoad()
{
	FString newName = FString("road spline Mesh : " + FString::FromInt(freeRoads.Num()));
	UFreeRoadGeneratorComponent* freeRoad = NewObject<UFreeRoadGeneratorComponent>(this, FName(newName));
	freeRoad->RegisterComponent();
	freeRoad->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
	freeRoad->SetRoadMesh(roadmesh);
	freeRoads.Add(freeRoad);

	return freeRoad;
}
