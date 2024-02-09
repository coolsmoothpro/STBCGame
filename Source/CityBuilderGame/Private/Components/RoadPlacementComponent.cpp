// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/RoadPlacementComponent.h"
#include "controllers/CBGPlayerController.h"
#include "Actors/GridManager.h"
#include "CityBuilderGame/CityBuilderGame.h"

// Sets default values for this component's properties
URoadPlacementComponent::URoadPlacementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URoadPlacementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...


}

ACBGPlayerController* URoadPlacementComponent::GetPlayerController() const
{
	if (!GetOwner() || !Cast<APawn>(GetOwner())) return nullptr;
	return Cast<ACBGPlayerController>(Cast<APawn>(GetOwner())->GetController());
}


// Called every frame
void URoadPlacementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RoadPlacement_Update();
	CalculateSelectedRoads();

}

void URoadPlacementComponent::RoadPlacement_Start()
{
	
	if (GetPlayerController()) {
		FHitResult h;
		if(GetPlayerController()->GetHitResultUnderCursor(ECC_Visibility ,0 , h) && GetPlayerController()->GetGridManager()){
			_roadplacement_StartCell =  GetPlayerController()->GetGridManager()->LocationToCell(h.ImpactPoint);
			bRoadPlacementStarted = true;
		}
	}

}

void URoadPlacementComponent::RoadPlacement_Update()
{
	if (!bRoadPlacementStarted) return;

	if (GetPlayerController()) {
		FHitResult h;
		if (GetPlayerController()->GetHitResultUnderCursor(ECC_Visibility, 0, h) && GetPlayerController()->GetGridManager()) {
			_roadplacement_CurrentCell = GetPlayerController()->GetGridManager()->LocationToCell(h.ImpactPoint);
		}
	}


}

void URoadPlacementComponent::RoadPlacement_Accept()
{

	if(GetPlayerController() && GetPlayerController()->GetGridManager()) {
		GetPlayerController()->GetGridManager()->OnRoadPlacement(CalculateSelectedRoads(1));
		GetPlayerController()->GetGridManager()->ClearCellDrawing(11);
	}
	
	bRoadPlacementStarted = false;

}

void URoadPlacementComponent::RoadPlacement_Cancel()
{
	bRoadPlacementStarted = false;
}


FGridCell Las_drawn_StartCell = FGridCell(), Las_drawn_EndCell = FGridCell();

TArray<struct FGridCell> URoadPlacementComponent::CalculateSelectedRoads(bool bCalculateInAnyCase )
{
	if (!bRoadPlacementStarted) return TArray<FGridCell>();


	if (Las_drawn_StartCell == _roadplacement_StartCell && Las_drawn_EndCell == _roadplacement_CurrentCell && !bCalculateInAnyCase) return TArray<FGridCell>();

	Las_drawn_StartCell = _roadplacement_StartCell;
	Las_drawn_EndCell = _roadplacement_CurrentCell;


	TArray<FGridCell> cells;

	if (_roadplacement_CurrentCell == _roadplacement_StartCell) {
		print("road cells : Same !");

		// no road drawing !

		return TArray<FGridCell>();
	}
	else if(_roadplacement_CurrentCell.x == _roadplacement_StartCell.x){

		int32 NumCellsNeeded = _roadplacement_CurrentCell.y- _roadplacement_StartCell.y;
		
		bool bMoveLower = _roadplacement_StartCell.y > _roadplacement_CurrentCell.y;
		for (int32 i = 0; i < FMath::Abs(NumCellsNeeded)+1; i++) {
			FGridCell cell = FGridCell(_roadplacement_StartCell.x, _roadplacement_StartCell.y + (i * (bMoveLower ? -1 : 1)) );
			cells.Add(cell);
		}

	}
	else if (_roadplacement_CurrentCell.y == _roadplacement_StartCell.y) {

		int32 NumCellsNeeded = _roadplacement_CurrentCell.x - _roadplacement_StartCell.x;

		bool bMoveLower = _roadplacement_StartCell.x > _roadplacement_CurrentCell.x;
		for (int32 i = 0; i < FMath::Abs(NumCellsNeeded) + 1; i++) {
			FGridCell cell = FGridCell(_roadplacement_StartCell.x + (bMoveLower ? -i : i), _roadplacement_StartCell.y);
			cells.Add(cell);
		}

	}
	else {
		
		int32 x_defference = _roadplacement_StartCell.x -_roadplacement_CurrentCell.x;
		int32 y_defference = _roadplacement_StartCell.y -_roadplacement_CurrentCell.y;

			print(FString::FromInt(FMath::Abs(x_defference)).Append(" ,    y difference = ").Append(FString::FromInt(FMath::Abs(y_defference))),455);
		
		if (FMath::Abs(y_defference) > FMath::Abs(x_defference)) {


			int32 NumCellsNeeded = _roadplacement_StartCell.y - _roadplacement_CurrentCell.y;

			bool bMoveLower = _roadplacement_StartCell.y > _roadplacement_CurrentCell.y;

			for (int32 i = 0; i < FMath::Abs(NumCellsNeeded) + 1; i++) {
				FGridCell cell = FGridCell(_roadplacement_StartCell.x, _roadplacement_StartCell.y + (bMoveLower ? -i : i));
				cells.Add(cell);
			}

			print("y > x", 12122);


		}
		else {
			print(" x > y", 12122);
			int32 NumCellsNeeded = _roadplacement_StartCell.x - _roadplacement_CurrentCell.x;
			bool bMoveLower = _roadplacement_StartCell.x > _roadplacement_CurrentCell.x;
			for (int32 i = 0; i < FMath::Abs(NumCellsNeeded) + 1; i++) {
				FGridCell cell = FGridCell(_roadplacement_StartCell.x + (bMoveLower ? -i : i), _roadplacement_StartCell.y);
				cells.Add(cell);
			}

		}

	}



	if (GetPlayerController() && GetPlayerController()->GetGridManager()) {
		GetPlayerController()->GetGridManager()->DrawCells(cells,11);
	}

	return cells;

}

void URoadPlacementComponent::PrintCell()
{
	if (GetPlayerController()) {
		FHitResult h;
		if (GetPlayerController()->GetHitResultUnderCursor(ECC_Visibility, 0, h) && GetPlayerController()->GetGridManager()) {
			FGridCell c = GetPlayerController()->GetGridManager()->LocationToCell(h.ImpactPoint);
			GetPlayerController()->GetGridManager()->DrawCells({c});
			print(c.ToString());
		}
	}
}

