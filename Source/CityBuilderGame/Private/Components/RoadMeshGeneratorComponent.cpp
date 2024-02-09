// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RoadMeshGeneratorComponent.h"
#include "Actors/GridManager.h"
#include "KismetProceduralMeshLibrary.h"
#include "CityBuilderGame/CityBuilderGame.h"


// Called when the game starts
void URoadMeshGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();
	

}


AGridManager* URoadMeshGeneratorComponent::GetGridManager()
{
	return Cast<AGridManager>(GetOwner());
}

// Called every frame
void URoadMeshGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URoadMeshGeneratorComponent::UpdateRoadMesh(TArray<struct FGridCell> roadcells)
{
	if (!GetGridManager()) return;


	for (AActor* c : spawnedActors) {
		c->Destroy();
	}
	spawnedActors.Empty();



	for (FGridCell c : roadcells) {
		if (GetGridManager()->IsRoadCell(c)) {
			FTransform trans;
			UStaticMesh* mesh = nullptr;
			MeshAndRotationForRoadCell(c, mesh, trans);

			ARoadActor* ac = GetWorld()->SpawnActor<ARoadActor>(ARoadActor::StaticClass() , trans);
			ac->SetMesh(mesh);
			spawnedActors.Add(ac);
		}
	}




}

void URoadMeshGeneratorComponent::MeshAndRotationForRoadCell(FGridCell& cell, UStaticMesh*& meshRef, FTransform& roadTransform)
{
	if (!GetGridManager()) return;

	bool bHasForwardNeighborCell = GetGridManager()->IsRoadCell(cell.GetNeighborCell(EGridCellDirection::EGCD_Forward));
	bool bHasBackNeighborCell = GetGridManager()->IsRoadCell(cell.GetNeighborCell(EGridCellDirection::EGCD_Back));
	bool bHasRightNeighborCell = GetGridManager()->IsRoadCell(cell.GetNeighborCell(EGridCellDirection::EGCD_Right));
	bool bHasLeftNeighborCell = GetGridManager()->IsRoadCell(cell.GetNeighborCell(EGridCellDirection::EGCD_Left));


	roadTransform.SetLocation(GetGridManager()->CellToLocation(cell,1));

	if (bHasForwardNeighborCell && bHasBackNeighborCell && bHasRightNeighborCell && bHasLeftNeighborCell) {
		
		//
		// X shape
		//
		
		meshRef = Road_Mesh_X;

	}
	else if (bHasForwardNeighborCell && bHasBackNeighborCell  && bHasRightNeighborCell ||
			 bHasBackNeighborCell    && bHasRightNeighborCell && bHasLeftNeighborCell    ||
			 bHasForwardNeighborCell && bHasRightNeighborCell && bHasLeftNeighborCell ||
			 bHasForwardNeighborCell && bHasBackNeighborCell  && bHasLeftNeighborCell   ) {
	
		//
		//  T Shape
		//
		
		meshRef = Road_Mesh_T;

		if (bHasForwardNeighborCell && bHasBackNeighborCell && bHasRightNeighborCell) {
			roadTransform.SetRotation(FRotator(0, 180, 0).Quaternion());
		}
		else if (bHasForwardNeighborCell && bHasRightNeighborCell && bHasLeftNeighborCell) {
			roadTransform.SetRotation(FRotator(0, 90, 0).Quaternion());
		}
		else if (bHasBackNeighborCell && bHasRightNeighborCell && bHasLeftNeighborCell) {
			roadTransform.SetRotation(FRotator(0, -90, 0).Quaternion());
		}

	}
	else if(bHasForwardNeighborCell && bHasRightNeighborCell ||
			bHasForwardNeighborCell && bHasLeftNeighborCell || 
			bHasBackNeighborCell && bHasRightNeighborCell  ||
		    bHasBackNeighborCell && bHasLeftNeighborCell   
			){
		
		//
		// L Shape
		//

		meshRef = Road_Mesh_L;

		if (bHasForwardNeighborCell && bHasRightNeighborCell) {
			roadTransform.SetRotation(FRotator(0,90, 0).Quaternion());
		}
		else if (bHasBackNeighborCell && bHasLeftNeighborCell) {
			roadTransform.SetRotation(FRotator(0, -90, 0).Quaternion());
		}
		else if (bHasBackNeighborCell && bHasRightNeighborCell) {
			roadTransform.SetRotation(FRotator(0, 180, 0).Quaternion());

		}

	}
	else {

		//
		// straight shape 
		//

		if (bHasRightNeighborCell || bHasLeftNeighborCell) {
			roadTransform.SetRotation(FRotator(0, 90, 0).Quaternion());
		}
		meshRef = Road_Mesh_Straight;
	}
}

