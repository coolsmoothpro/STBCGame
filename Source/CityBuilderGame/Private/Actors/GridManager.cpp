
#include "Actors/GridManager.h"
#include "Actors/BuildingBase.h"
#include "ProceduralMeshComponent.h"
#include "Components//RoadMeshGeneratorComponent.h"
#include "CityBuilderGame/CityBuilderGame.h"

AGridManager::AGridManager()
{
	if (!ProceduralMesh) {
		ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(FName("mesh"));
		ProceduralMesh->SetupAttachment(GetRootComponent());
	}
	if (!RoadMeshGenerator) {
		 RoadMeshGenerator = CreateDefaultSubobject<URoadMeshGeneratorComponent>(FName("Road Mesh"));
	}

}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	SetupGrid();
}

void AGridManager::SetupGrid()
{
	int numCellsNeeded = _gridSettings.Num_Grid_Cells * _gridSettings.Num_Grid_Cells;
	GridCells.Reserve(numCellsNeeded);
}

FGridCell& AGridManager::GetCell(int x, int y)
{
	if (x < 0 || y < 0 || x >= _gridSettings.Num_Grid_Cells || y >= _gridSettings.Num_Grid_Cells) return GridCells[_gridSettings.Num_Grid_Cells* _gridSettings.Num_Grid_Cells];
	int32 Index = _gridSettings.Num_Grid_Cells * y + x;
	
	if (!GridCells.IsValidIndex(Index)) {
		GridCells.Insert(Index,FGridCell(x,y));
	}

	return GridCells[Index];
}



void AGridManager::Init(UPlacementComponent* _Owner)
{

}

void AGridManager::SetGridSettingsData(FGridSettingsData data)
{
	_gridSettings = data;

}



void AGridManager::ApplyGridSettings(FTransform& transform)
{

	FVector location = transform.GetLocation();
	int32 XCells = location.X / _gridSettings.GridSize;
	int32 YCells = location.Y / _gridSettings.GridSize;

	location.X = XCells * _gridSettings.GridSize;
	location.Y = YCells * _gridSettings.GridSize;

	if (_gridSettings.bSnapToGridCellCenter) {
		location.X += _gridSettings.GridSize / 2.0f;
		location.Y += _gridSettings.GridSize / 2.0f;
	}

	transform.SetLocation(location);
}

TArray<FGridCell> AGridManager::GetCellsNeededForBuilding(ABuildingBase* actorToPlace)
{
	FVector location = actorToPlace->GetActorLocation();
	int32 XCells = location.X / _gridSettings.GridSize;
	int32 YCells = location.Y / _gridSettings.GridSize;

	
	TArray<FGridCell> cellsNeeded;
	for (size_t i = 0; i < actorToPlace->width_Cell; i++) {
		for (size_t j = 0; j < actorToPlace->depth_Cell; j++) {

			FGridCell cell;
			// cells based on rotation
			if (FMath::IsNearlyEqual(actorToPlace->GetActorRotation().Yaw, 0 , 0.5)) {
				cell = FGridCell(XCells + i, YCells + j);
			}
			else if (FMath::IsNearlyEqual(actorToPlace->GetActorRotation().Yaw, -90,0.5)) {
				cell = FGridCell(XCells + j, YCells - i -1);
			}
			else if (FMath::IsNearlyEqual(actorToPlace->GetActorRotation().Yaw, 180, 0.5))
			{
				cell = FGridCell(XCells - i -1 , YCells - j -1);
			}
			else {
				cell = FGridCell(XCells - j -1 , YCells + i);
			}

			cellsNeeded.Add(cell);
		}
	}

	return cellsNeeded;
}

/*

if ((IsCellValid() && GetCell(c2.x, c2.y).IsEqual(c2) && !GetCell(c2.x, c2.y).IsTheSameReserver(actorToPlace->GetBuildingId()))) {
			return false;
		}

*/

bool AGridManager::CanAddBuildingToGrid(ABuildingBase* actorToPlace)
{
	if (!actorToPlace) { print("actor to Place  is null "); return false; };

	TArray<FGridCell> cellsNeeded = GetCellsNeededForBuilding(actorToPlace);

	
	for (FGridCell c : cellsNeeded) {
		if (IsCellReserved(c) && !GetCell(c.x,c.y).IsTheSameReserver(actorToPlace->GetBuildingId())) {
			return false;
		}
	}

	return true;
}

FGridCell AGridManager::LocationToCell(FVector Location)
{
	int32 XCells = 0;
	int32 YCells = 0;

	XCells = Location.X / _gridSettings.GridSize;
	YCells = Location.Y / _gridSettings.GridSize;

	if (Location.X < 0) {
		XCells -= 1;
	}	
	
	if (Location.Y < 0) {
		YCells -= 1;
	}

	return FGridCell(XCells, YCells);
}

FVector AGridManager::CellToLocation(FGridCell cell, bool bCellCenter)
{
	return FVector(cell.x * _gridSettings.GridSize + _gridSettings.GridSize/2 , cell.y * _gridSettings.GridSize + _gridSettings.GridSize/2 , 60);
}

bool AGridManager::AddBuildingToGrid(ABuildingBase* ToPlace)
{
	if (!ToPlace) return false;


	if (CanAddBuildingToGrid(ToPlace)) {
		TArray<FGridCell> neededcells = GetCellsNeededForBuilding(ToPlace);
		ToPlace->SetReserverCells(neededcells);
		for (FGridCell& c : neededcells) {
			if (IsCellAllocated(c)) {
				// reserve
				GetCell(c.x, c.y).Reserve(ToPlace->GetBuildingId());
			}
			else {
				// make new and reserve new cells !
				GetCell(c.x, c.y) = FGridCell(c.x,c.y);
				GetCell(c.x, c.y).Reserve(ToPlace->GetBuildingId());
			}
		}
	}

	return true;
}

void AGridManager::ReplaceBuilding(ABuildingBase* ToPlace)
{
	if (!ToPlace) return;

	for (FGridCell c : ToPlace->GetReservedCells()) {
		GetCell(c.x,c.y).Release();
	}

	if (CanAddBuildingToGrid(ToPlace)) {
		TArray<FGridCell> cells = GetCellsNeededForBuilding(ToPlace);
		ToPlace->SetReserverCells(cells);
		for (FGridCell c : cells) {
				GetCell(c.x, c.y).Reserve(ToPlace->GetBuildingId());

		}
	}

}

void AGridManager::OnBuildingRemoved(ABuildingBase* ToRemove)
{
	// free reserved cells
	for (FGridCell c : ToRemove->GetReservedCells()) {
			GetCell(c.x,c.y).Release();
	}
}

void AGridManager::OnRoadPlacement(TArray<struct FGridCell> Cells)
{
	// all cells are available for road


	for (FGridCell& c : Cells) {
		if (!IsCellValid(c)) {
			print_Error("Cell is not valid ! ");
			continue;
		}

			if (IsCellAllocated(c)) {
				if (!IsCellReserved(c)) {
					GetCell(c.x, c.y).Reserve_road();
				}
			}
			else {
				// init
				GetCell(c.x, c.y) = FGridCell(c.x,c.y);
				GetCell(c.x, c.y).Reserve_road();
					if (IsCellAllocated(GetCell(c.x, c.y))) {
					}

			}
		roadCells.Add(c);
	}


	if (RoadMeshGenerator) {
		RoadMeshGenerator->UpdateRoadMesh(roadCells);
	}

}

#pragma region Draw Cell for prevew


FGridCell LastdrawnLocation;
float lastYaw = 0;
void AGridManager::RedrawPlacementCells(ABuildingBase* toPlace)
{

	// check if this building in this particular location is already drawn preview cells !
	if (LocationToCell(toPlace->GetActorLocation()).IsEqual(LastdrawnLocation) && toPlace->GetBuildingId() == LastdrawnLocation.reserverBuildingId && FMath::IsNearlyEqual(lastYaw,toPlace->GetActorRotation().Yaw,0.5)) return;
	LastdrawnLocation = LocationToCell(toPlace->GetActorLocation());
	LastdrawnLocation.reserverBuildingId = toPlace->GetBuildingId();
	lastYaw = toPlace->GetActorRotation().Yaw;

	ClearCellDrawing(0);
	DrawCells(GetCellsNeededForBuilding(toPlace), 0, grid_preview_padding, Grid_placment_Previrew_material, FVector(0, 0, toPlace->GetActorLocation().Z + 2));
}


/*
	data for drawing a cell
*/
struct DrawCellData {
	DrawCellData() {}
	DrawCellData(TArray<FVector> _vectors, TArray<int> _tringles) {
		tringles = _tringles;
		vectors = _vectors;
	}
	TArray<FVector> vectors;
	TArray<int> tringles;

	/*
	returns vectors and trignles for each cell !
	*/
	static DrawCellData MakeCellDrawData(FGridCell cell, float cell_size, FVector offset = FVector(), float padding = 10.0)
	{
		TArray<FVector> vectors = {
			FVector(cell_size * cell.x + padding ,cell_size * cell.y + padding,0) + offset ,
			FVector(cell_size * cell.x + cell_size - padding,cell_size * cell.y + padding,0) + offset ,
			FVector(cell_size * cell.x + cell_size - padding,cell_size * cell.y + cell_size - padding,0) + offset ,
			FVector(cell_size * cell.x + padding ,cell_size * cell.y + cell_size - padding,0) + offset };
		TArray<int> tringles = { 2,1,0   ,0,3,2 };

		return DrawCellData(vectors, tringles);
	}
};
// data for drawing multiple cells / mesh 
struct DrawData {
	DrawData() {

	}
	TArray<FVector> vectors;
	TArray<int> tringles;

	static DrawData MakeSetOfCells(TArray<FGridCell> cells, float cell_size, FVector offset = FVector(), float padding = 10.0) {
		TArray<DrawCellData> CellsData;
		for (FGridCell c : cells) {
			CellsData.Add(DrawCellData::MakeCellDrawData(c, cell_size, offset, padding));
		}
		DrawData drawData = DrawData();

		for (int i = 0; i < CellsData.Num(); i++) {
			for (int j = 0; j < CellsData[i].vectors.Num(); j++) {
				drawData.vectors.Add(CellsData[i].vectors[j]);
			}
			for (int j = 0; j < CellsData[i].tringles.Num(); j++) {
				drawData.tringles.Add(CellsData[i].tringles[j] + 4 * i);
			}
		}
		return drawData;
	}
};

void AGridManager::DrawCells(TArray<FGridCell> cells, int meshIndex, float Padding, UMaterialInterface* CustomCellDrawMaterial, FVector offset)
{
	DrawData d = DrawData::MakeSetOfCells(cells, _gridSettings.GridSize, offset, Padding);

	if (ProceduralMesh) {
		ProceduralMesh->CreateMeshSection(meshIndex, d.vectors, d.tringles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), 0);
		ProceduralMesh->SetMaterial(meshIndex, CustomCellDrawMaterial);

	}

}

void AGridManager::ClearCellDrawing(int Meshindex, bool bAllSections)
{
	if (!ProceduralMesh) return;
	if (bAllSections) {
		ProceduralMesh->ClearAllMeshSections();
	}
	else {
		ProceduralMesh->ClearMeshSection(Meshindex);
	}

}

#pragma endregion

// Getters

int32 AGridManager::GridCellAsIndex(int x, int y) const
{
	if (x < 0 || y <0 || x  > _gridSettings.Num_Grid_Cells - 1 || y > _gridSettings.Num_Grid_Cells - 1) return -1;
	return _gridSettings.Num_Grid_Cells *y+x;
}

bool AGridManager::IsCellValid(int x, int y)
{
	return GridCellAsIndex(x, y) != -1;
}

bool AGridManager::IsCellValid(FGridCell cell)
{
	if (!cell.IsValid()) return false;
	return IsCellValid(cell.x,cell.y);
}

bool AGridManager::IsCellAllocated(int x, int y)
{
	if (!IsCellValid(x, y)) return false;
	return GridCells.IsValidIndex(GridCellAsIndex(x,y));
}

bool AGridManager::IsCellAllocated(FGridCell cell)
{
	return IsCellAllocated(cell.x,cell.y);
}

bool AGridManager::IsCellReserved(int x, int y)
{
	if (!IsCellAllocated(x, y)) return false;
	return !GridCells[GridCellAsIndex(x, y)].IsFree();
}

bool AGridManager::IsCellReserved(FGridCell cell)
{
	return IsCellReserved(cell.x,cell.y);
}

bool AGridManager::IsRoadCell(int x, int y)
{
	if (!IsCellReserved(x, y)) return false;

	return GetCell(x,y).IsRoad();
}

bool AGridManager::IsRoadCell(FGridCell cell)
{
	return IsRoadCell(cell.x,cell.y);
}

