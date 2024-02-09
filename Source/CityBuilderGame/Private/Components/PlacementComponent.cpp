

#include "Components/PlacementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Actors/BuildingBase.h"
#include "Actors/GridManager.h"
#include "CityBuilderGame/CityBuilderGame.h"





// Sets default values for this component's properties
UPlacementComponent::UPlacementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlacementComponent::BeginPlay()
{
	Super::BeginPlay();

	if (CustomGridClass) {
		_GridManager = GetWorld()->SpawnActor<AGridManager>(CustomGridClass);
		_GridManager->Init(this);
	}
	else {
		_GridManager = GetWorld()->SpawnActor<AGridManager>();
		_GridManager->Init(this);
		_GridManager->SetGridSettingsData(GridSettings);
	}

}


// Called every frame
void UPlacementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bstartPlacing) {
		

		// add ofset for the cursor location so the building will appear in the center of the mouse (not in a cornor)
		FVector cursorOffset = FVector(_previewPlacement->width_Cell * 100 / 2, _previewPlacement->depth_Cell * 100 / 2, 0);

		FTransform transform;
		if (GetUnderCursorTrans(transform , cursorOffset)) {
			// apply offset to keep the building in the center of the mouse even if rotated
			transform.SetLocation(transform.GetLocation());
			UpdatePreviewMewsh(transform);
			_GridManager->RedrawPlacementCells(_previewPlacement);

		}
	}
	else if (bIsReplacing) {
		// add ofset for the cursor location so the building will appear in the center of the mouse (not in a cornor)
		FVector cursorOffset = FVector(_SelectedPlacement->width_Cell * 100 / 2, _SelectedPlacement->depth_Cell * 100 / 2, 0);

		FTransform transform;
		if (GetUnderCursorTrans(transform , cursorOffset)) {
			UpdateSelectedForReplacement(transform);
			_GridManager->RedrawPlacementCells(_SelectedPlacement);
		}

	}
	else {
		// track under the mouse 
		CheckUnderCursorForSelection();
	}


}

APlayerController* UPlacementComponent::GetPlayerController() const
{

	if (Cast<APawn>(GetOwner())) {
		return Cast<APlayerController>(Cast<APawn>(GetOwner())->GetController());
	}

	print_Error("The Owner Is Not a Pawn !   UPlacementComponent");
	return nullptr;

}


bool UPlacementComponent::GetUnderCursorTrans(FTransform& UnderCursorTrans , FVector CursorOffset)
{
	if (GetPlayerController()) {
		FHitResult h;
		if (GetPlayerController()->GetHitResultUnderCursor(ECC_Camera, 0, h)) {
			
			UnderCursorTrans.SetLocation(h.ImpactPoint - CursorOffset);


			if (_GridManager) {
				_GridManager->ApplyGridSettings(UnderCursorTrans);
			}

			if (CurrentPlacementData.bAlignToSurfaceNormal) {
				UnderCursorTrans.SetRotation(UKismetMathLibrary::MakeRotFromZX(h.ImpactNormal, FVector::ForwardVector).Quaternion());
			}
			else {
				UnderCursorTrans.SetRotation(FQuat::Identity);
			}

			return 1;
		}
	}

	return false;
}

void UPlacementComponent::UpdatePreviewMewsh(FTransform PlacementTransform)
{
	// already spawned  just re possition it
	if (_previewPlacement && bstartPlacing) {

		// apply offset befor updating location and rotation
		PlacementTransform.SetLocation(PlacementTransform.GetLocation() + CurrentPlacementData.LocationOffset);
		FRotator rot = PlacementTransform.GetRotation().Rotator();
		rot.Yaw += CurrentPlacementData.Additional_Placement_time_Yaw;
		PlacementTransform.SetRotation(rot.Quaternion());
		_previewPlacement->SetActorTransform(PlacementTransform);



		// update placement State
		if (_GridManager && _previewPlacement) {
			if (_GridManager->CanAddBuildingToGrid(_previewPlacement)) {
				_previewPlacement->OnPlacementProcessValidation(GetWorld()->GetDeltaSeconds(), true, 1);
			}
			else {
				_previewPlacement->OnPlacementProcessValidation(GetWorld()->GetDeltaSeconds(), false, 1);
			}
		}

	}
}

void UPlacementComponent::UpdateSelectedForReplacement(FTransform PlacementTransform)
{
	if (_SelectedPlacement && bIsReplacing) {
		
		FRotator rot = PlacementTransform.GetRotation().Rotator();
		rot.Yaw += CurrentPlacementData.Additional_Placement_time_Yaw;
		PlacementTransform.SetRotation(rot.Quaternion());

		_SelectedPlacement->SetActorTransform(PlacementTransform);
		if (_GridManager->CanAddBuildingToGrid(_SelectedPlacement)) {
			_SelectedPlacement->OnPlacementProcessValidation(GetWorld()->GetDeltaSeconds(), true, 0);
		}
		else {
			_SelectedPlacement->OnPlacementProcessValidation(GetWorld()->GetDeltaSeconds(), false, 0);
		}
	}
}
void UPlacementComponent::Placement_Start(FPlacementData data)
{
	if (bstartPlacing) return;

	// unselect already selected placment
	if (_SelectedPlacement) {
		_SelectedPlacement->OnUnSelected();
		_SelectedPlacement = nullptr;
	}

	CurrentPlacementData = data;
	if (!_previewPlacement) {
		if (_previewPlacement = GetWorld()->SpawnActor<ABuildingBase>(data.ActorToPlace)) {
			_previewPlacement->OnPreviewPlacement();
			bstartPlacing = true;
		}
	}
}

void UPlacementComponent::Placement_Accept()
{
	if (!bstartPlacing || !_GridManager || !_GridManager->CanAddBuildingToGrid(_previewPlacement)) {
		Placement_Cancel();
		return;
	}

	bstartPlacing = false;
	if (_previewPlacement) {
		_previewPlacement->OnPlaced(this);
		_GridManager->AddBuildingToGrid(_previewPlacement);
		_GridManager->ClearCellDrawing(0);
		_previewPlacement = nullptr;
	}

}
void UPlacementComponent::Placement_Cancel()
{
	if (!bstartPlacing) return;
	bstartPlacing = false;

	if (_previewPlacement) {
		_previewPlacement->Destroy();
		_GridManager->ClearCellDrawing(0);
		_previewPlacement = nullptr;
	}
}


void UPlacementComponent::ApplyYawRotationToPlacement(TEnumAsByte<ERotationDirection> Direction)
{
	if (!_previewPlacement) return;

	float RotationValue = 0;
	
	switch (Direction)
	{
		case ERD_clockwise: 
			CurrentPlacementData.Additional_Placement_time_Yaw += 90;
			break;
		case ERD_Counterclockwise : 
			CurrentPlacementData.Additional_Placement_time_Yaw += -90;
			break;
	}

	// update offset to fix rotation (to keep the building in the cursor center)
	// cells based on rotation
	if (FMath::IsNearlyEqual(_previewPlacement->GetActorRotation().Yaw, 0, 0.5)) {
		BuidlingOffset_For_YawRotation = FVector();
	}
	else if (FMath::IsNearlyEqual(_previewPlacement->GetActorRotation().Yaw, -90, 0.5)) {
		BuidlingOffset_For_YawRotation = FVector();
	}
	else if (FMath::IsNearlyEqual(_previewPlacement->GetActorRotation().Yaw, 180, 0.5))
	{
		//BuidlingOffset_For_YawRotation = FVector(_previewPlacement->depth_Cell * 50,0,0);
	}
	else {
		BuidlingOffset_For_YawRotation = FVector();
	}

	 
}

bool UPlacementComponent::CheckUnderCursorForSelection()
{
	if (!GetPlayerController()) return false;

	FHitResult h;
	if (GetPlayerController()->GetHitResultUnderCursor(ECC_WorldStatic, 0, h)) {

		if (ABuildingBase* hitted = Cast<ABuildingBase>(h.GetActor())) {

			// hover when there is no hovered building
			if (!_HoveredPlacement) {
				_HoveredPlacement = hitted;
				_HoveredPlacement->OnHovered();

			}
			// when there is already hovered building and hovered a new one 
			else if (_HoveredPlacement && _HoveredPlacement != hitted) {
				_HoveredPlacement->OnUnHovered();
				_HoveredPlacement = hitted;
				_HoveredPlacement->OnHovered();
			}

		}
		else {
			// unhover the already hovered placement buildings
			if (_HoveredPlacement) {
				_HoveredPlacement->OnUnHovered();
				_HoveredPlacement = nullptr;
			}

		}

	}


	return false;
}

void UPlacementComponent::SelectUnderCursor()
{
	//   placing new  OR   replacing  OR                clicking already slected building              
	if (bstartPlacing || bIsReplacing || (_HoveredPlacement && _SelectedPlacement == _HoveredPlacement)) return;


	if (_HoveredPlacement) {

		// if already seleced, just unselect
		if (_SelectedPlacement) {
			_SelectedPlacement->OnUnSelected();
			_SelectedPlacement = nullptr;
		}
		// selct newOne 
		_SelectedPlacement = _HoveredPlacement;
		_SelectedPlacement->OnSelected();

	}
	else if (_SelectedPlacement) {
		_SelectedPlacement->OnUnSelected();
		_SelectedPlacement = nullptr;
	}

}


bool UPlacementComponent::Replacement_Start(ABuildingBase* building)
{
	if (bIsReplacing || bstartPlacing || !building) return false;

	_Pre_Replace_Selected_Transform = building->GetActorTransform();
	bIsReplacing = true;
	_SelectedPlacement = building;
	_SelectedPlacement->OnReplace_Started();
	return true;
}

void UPlacementComponent::Replacement_Accept()
{
	if (!bIsReplacing || !_GridManager || !_GridManager->CanAddBuildingToGrid(_SelectedPlacement)) {
		Replacement_Cancel();
		return;
	}

	bIsReplacing = false;
	_SelectedPlacement->OnReplaced();
	_GridManager->ReplaceBuilding(_SelectedPlacement);
	_GridManager->ClearCellDrawing(0);
	_SelectedPlacement = nullptr;

}

void UPlacementComponent::Replacement_Cancel()
{
	if (!bIsReplacing) return;

	bIsReplacing = false;
	_SelectedPlacement->SetActorTransform(_Pre_Replace_Selected_Transform);
	_SelectedPlacement->OnReplaced();
	_GridManager->ClearCellDrawing(0);
	_SelectedPlacement = nullptr;

}


void UPlacementComponent::RemoveBuilding(ABuildingBase* toRemove)
{
	if (_SelectedPlacement == toRemove) {
		_SelectedPlacement = nullptr;
	}
	if (_HoveredPlacement == toRemove) {
		_HoveredPlacement = nullptr;
	}

	if (_GridManager) {
		_GridManager->OnBuildingRemoved(toRemove);
	}


	toRemove->Destroy();
}

