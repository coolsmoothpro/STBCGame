// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "utils/CBGUtils.h"
#include "CBGTypes.generated.h"



#pragma region Enums

UENUM(BlueprintType)
enum ERotationDirection {
	ERD_clockwise = 0    UMETA(DisplayName = "Clockwise"),
	ERD_Counterclockwise UMETA(DisplayName = "CounterClockwise"),
};

UENUM(BlueprintType)
enum EGridCellDirection{
	EGCD_Forward = 0    UMETA(DisplayName = "forward"),
	EGCD_Back           UMETA(DisplayName = "Back"),
	EGCD_Right          UMETA(DisplayName = "Right"),
	EGCD_Left           UMETA(DisplayName = "Left")

};




UENUM(BlueprintType)
enum ETools {
	ET_Select = 0       UMETA(DisplayName = "Select") ,
	ET_Place_Building   UMETA(DisplayName = "place building") ,
	ET_Replace          UMETA(DisplayName = "replace"),
	ET_Place_Road       UMETA(DisplayName = "place road"),
};




#pragma endregion


#pragma region Game Structs



/*
	is datas needed to start placment process
*/
USTRUCT(BlueprintType)
struct FPlacementData {
	GENERATED_BODY()

	// cpp only

	/*
		rotates the mesh after spawn around Z axis by this amount
	*/
	float Additional_Placement_time_Yaw = 0;


	// exposed to BP

	/*
		the item needed to be placed in the world
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement System")
	TSubclassOf<class ABuildingBase> ActorToPlace;
	/*
		whether to align th ebuilding to the normal of the surface or keep it facing Up Axis (Z)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement System")
	bool bAlignToSurfaceNormal = true;

	/*
		the positional offset to be applyed to the placement actor
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement System")
	FVector LocationOffset;

	/*
		whether to apply grid rule to location, the building can not be placed freely, it will snap to certain points based on grid settings
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement System")
	bool bApplyGridRules = true;


};


/*
	Settings used in grid proccessing
*/
USTRUCT(BlueprintType)
struct FGridSettingsData {
	GENERATED_BODY()

	FGridSettingsData() {}


	/*
		whether to snap the location to the center of the cell rather than corner of the grid cell
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement System")
	int32 Num_Grid_Cells = __Grid_Num_Cells;

	/*
		size (x,y) of the grids ( in cm )
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement System")
	double GridSize = __Grid_Size_Default;

	/*
		whether to snap the location to the center of the cell rather than corner of the grid cell
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement System")
	bool bSnapToGridCellCenter = false;


};



/*
	data for each cell in the grid
*/
USTRUCT(BlueprintType)
struct FGridCell {
	GENERATED_BODY()

private:
	bool bIsValid = false;
	bool bIsReserved = false;
	bool bIsRoad = false;
public:

	FGridCell() {}
	FGridCell(int32 _x, int32 _y) {
		x = _x;
		y = _y;
		bIsValid = true;
	}

	// x and y index of cells
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement System");
	int32 x;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement System");
	int32 y;

	FGuid reserverBuildingId;

	//static bool operator== (const FGridCell& A , const FGridCell& B) {
	//	return (B.x == A.x && B.y == A.y);
	//}

	// is the cell initalized
	bool IsValid()const { return bIsValid; }
	// is this cell already used
	bool IsFree()const { return !bIsReserved; }
	// is this road reserved for road
	bool IsRoad() const { return bIsReserved && bIsRoad; }

	void Reserve(FGuid _reserverBuildingId) {
		if (bIsReserved || !_reserverBuildingId.IsValid() || bIsRoad) return;

		reserverBuildingId = _reserverBuildingId;
		bIsReserved = true;
		bIsRoad = false;
	}
	void Release() {
		if (!bIsReserved) return;

		reserverBuildingId = FGuid();;
		bIsReserved = false;
		bIsRoad = false;
	}
	/*
		reserve for road
	*/
	void Reserve_road() {
		if (!IsValid() || bIsReserved ) return;
		bIsReserved = true;
		bIsRoad = true;
	}

	bool IsEqual(const FGridCell& cell) const{
		return (cell.x == x && cell.y == y);
	}

	bool IsTheSameReserver(FGuid NewreserverId) const {
		return reserverBuildingId == NewreserverId;
	}


	bool operator==(const FGridCell& other) const {
		return  other.x == x && other.y == y;

	}

	FGridCell GetNeighborCell(EGridCellDirection direction);

	FString ToString() { return FString("x= ").Append(FString::FromInt(x)).Append(" , y= ").Append(FString::FromInt(y)); }


};

#pragma endregion


#pragma region DATA and UI Structs


/*
	this strct used for building Inventory UI, represents a UI Button's data
*/
USTRUCT(BlueprintType)
struct FInventoryBuildingData : public FTableRowBase
{
	GENERATED_BODY()
	/*
		the class of the building that this button represents
	*/
	UPROPERTY(BlueprintReadWrite , EditAnywhere , Category = "building Inventory Data")
	TSubclassOf<class ABuildingBase> buildingClass;
	/*
		the name of the building
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "building Inventory Data")
	FString BuildingName;
	/*
		a preview image of the building
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "building Inventory Data")
	UTexture2D* image;
};


#pragma endregion

