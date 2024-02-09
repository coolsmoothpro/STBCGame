
#pragma once

#include "CoreMinimal.h"
#include "Gameframework/actor.h"
#include "types/CBGtypes.h"
#include "Containers/SparseArray.h"
#include "GridManager.generated.h"

class UPlacementComponent;


UCLASS()
class CITYBUILDERGAME_API AGridManager : public AActor
{
	GENERATED_BODY()

	AGridManager();


protected: 

	// Componwents
	 UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid  System")
	// for generating custom meshes in runtime
	class UProceduralMeshComponent* ProceduralMesh;

	// Componwents
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Grid  System")
	// for generating custom meshes in runtime
	class URoadMeshGeneratorComponent* RoadMeshGenerator;


	
	// settings for this grid 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, category = "grid Settings ")
	FGridSettingsData _gridSettings;

	// all cells reserved by buildings
	TSparseArray<FGridCell> GridCells;

	// all cells reserved by roads
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, category = "grid Settings | Cells")
	TArray<FGridCell> roadCells;

	/*
		material used for preview cells
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere , category = "grid Settings ")
	UMaterialInterface* Grid_placment_Previrew_material; 

	// the padding for grid preview (0 the preview will fill the entir grid, otherwise adds padding)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, category = "grid Settings ")
	float grid_preview_padding = 10;



	virtual void BeginPlay() override;



	void SetupGrid();
	FGridCell& GetCell(int x, int y);

public:

	// called after spawn to init
	void Init(UPlacementComponent* _Owner);
	// update grid settings data
	void SetGridSettingsData(FGridSettingsData data);

	// snaps the location acording to the grid settings
	void ApplyGridSettings(FTransform& transform);

	/*
		returns a list of all needed cells to be reserved for this building to be placed !
	*/
	TArray<struct FGridCell> GetCellsNeededForBuilding(ABuildingBase* actorToPlace);
	
	/*
		checks if this building can be placed on its current location
	*/
	bool CanAddBuildingToGrid(ABuildingBase* actorToPlace);

	/*
		converts location to cell
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid  System")
	FGridCell LocationToCell(FVector Location);
	/*
	   converts Cell to Location
	*/
	UFUNCTION(BlueprintCallable, Category = "Grid  System")
	FVector CellToLocation(FGridCell cell , bool bCellCenter);


	/*
		adds building to the grid for tyhe first time and reserves the reqyuired cells
	*/
	bool AddBuildingToGrid(class ABuildingBase* ToPlace);
	/*
		called when a building replaced, so the already reserved cells will be freed, and new ones will be reserved
	*/
	void ReplaceBuilding(class ABuildingBase* ToPlace);

	/*
		called when a building replaced, so the already reserved cells will be freed, and new ones will be reserved
	*/
	void OnBuildingRemoved(class ABuildingBase* ToRemove);


	void OnRoadPlacement(TArray<struct FGridCell> roadCells);


	/*
		draws cells as mesh, on the grid in their proper position
		can be used for any kind of drawing 
		for example used it to indicate the used/reserved cells in the grid !

		@param cells					list of cells to draw
		@param meshIndex				works as an id for sectoins you are drawing (cells you are drawing)
		@param Padding					is the padding for drawing cells (bigger the padding, smaller cell will be drawed) (please make a quick search to see how padding works)
		@param CustomCellDrawMaterial   material used while drawing the cells (mesh)
		@param offset					offset to be applyed to the drawing mesh	 

	*/
	UFUNCTION(BlueprintCallable, Category = "Grid System")
	void DrawCells(TArray<FGridCell> cells , int meshIndex = 1 , float Padding = 10.0f, UMaterialInterface* CustomCellDrawMaterial = nullptr , FVector offset = FVector(0,0,60));
	/*
		re-draws cells for a praticular building
	*/
	void RedrawPlacementCells(class ABuildingBase* ToPlace);
	UFUNCTION(BlueprintCallable, Category = "Grid  System")
	// clears already drawn cells
	void ClearCellDrawing(int meshIndex, bool bAllSections = false);




	// Getters

	const TSparseArray<FGridCell>& GetGridCells() { return GridCells; }
	
	/*
		returns the road mesh generator component
	*/
	class URoadMeshGeneratorComponent* GetRoadMeshGenerator() const { return RoadMeshGenerator; }
	


	/*
		converts Cell x and y components into an index to work with @GridCell
	*/
	int32 GridCellAsIndex(int x, int y) const;
	/*
		returns true if the cell is valid (in term of location)
	*/

	bool IsCellValid(int x, int y);
	bool IsCellValid(FGridCell cell);

	/*
		returns true if the cell is allocated
	*/
	bool IsCellAllocated(int x,int y);
	bool IsCellAllocated(FGridCell cell);

	/*
		returns true if the cell is already used for replacement (building, road etc)
	*/
	bool IsCellReserved(int x,int y);
	bool IsCellReserved(FGridCell cell);

	/*
		returns true if the cell is already reserved/used for road
	*/
	bool IsRoadCell(int x, int y);
	bool IsRoadCell(FGridCell cell);




};
