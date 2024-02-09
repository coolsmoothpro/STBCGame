// Fill out your copyright notice in the Description page of Project Settings.


#include "types/CBGTypes.h"

FGridCell FGridCell::GetNeighborCell(EGridCellDirection direction)
{
	switch (direction)
	{
	case EGCD_Forward:
		return FGridCell(x + 1,y);
		break;
	case EGCD_Back:
		return FGridCell(x - 1, y);
		break;
	case EGCD_Right:
		return FGridCell(x, y + 1);
		break;
	case EGCD_Left:
		return FGridCell(x, y - 1);
		break;
	default:
		return *this;
		break;
	}
}
