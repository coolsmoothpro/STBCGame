// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "types/CBGTypes.h"
#include "PlacementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CITYBUILDERGAME_API UPlacementComponent : public UActorComponent
{
	GENERATED_BODY()


	// Internal Variables
	class UCameraComponent* _camera;
	/*
		the placement actor spawned to the world as preview and its placement not yet been accepted
	*/
	class ABuildingBase* _previewPlacement;

	/*
		the placement actor spawned to the world as preview and its placement not yet been accepted
	*/
	class ABuildingBase* _HoveredPlacement;

	/*
		the placement actor spawned to the world as preview and its placement not yet been accepted
	*/
	class ABuildingBase* _SelectedPlacement;

	/*
		the current(last) data used for placement
	*/
	 struct FPlacementData CurrentPlacementData;

protected:
	/*
		the placement actor spawned to the world as preview and its placement not yet been accepted
	*/
	class AGridManager* _GridManager;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement Component | Grid")
	TSubclassOf<class AGridManager>CustomGridClass;


	/*
		settings for grid processing
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Placement Component | Grid")
	struct FGridSettingsData GridSettings;

	// internal functions

	// returns player controller of this player
	class APlayerController* GetPlayerController()const;

	// Called when the game starts
	virtual void BeginPlay() override;

	//UPROPERTY(BlueprintReadWrite , EditAnywhere , Category = "Placement System | ")

	// is started placing buildings
	bool bstartPlacing = false;
	// is has a selected building to replace
	bool bIsReplacing = false;
	// the selected builing for replacing, the transform befor starting to replace (if replacement canceled this will be used to reset transform)
	FTransform _Pre_Replace_Selected_Transform;
	// the location offset to be applyed to the _previewPlacement/_selectedPlacement because of YAW rotation (we apply offset to keep the building in the center of the cursor even if rotation applyed) 
	FVector BuidlingOffset_For_YawRotation;

public:
	// Sets default values for this component's properties
	UPlacementComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/*
		returns transform under the cursor
		applys grid rules acording to @GridSettings

		@param UnderCursorLoc the transform under the cursor
		@param CursorOffset apply offset to cursor
	*/
	UFUNCTION(BlueprintCallable, Category = "Placement Component")
	bool GetUnderCursorTrans(FTransform& UnderCursorLoc , FVector CursorOffset);


	/*
		updates the spawned Placment mesh's location and rotation, this will be called when the placment started but not yet accepted or canceled !
	*/
	void UpdatePreviewMewsh(FTransform PlacementTransform);


	/*
		updates the a selected buildings location and rotation,!
	*/
	void UpdateSelectedForReplacement(FTransform PlacementTransform);


	/*
		can be called to start placing the passed static mesh ( @mesh ) to the world !
	*/
	UFUNCTION(BlueprintCallable, Category = "Placement System")
	void Placement_Start(struct FPlacementData Data);

	/*
		this can be called to cancel placement process (if already started)
	*/
	UFUNCTION(BlueprintCallable, Category = "Placement System")
	void Placement_Cancel();

	/*
		this can be called to verify and accept placement process (if already started)
		this request will be ignored if the the placment conditions does not match !
	*/
	UFUNCTION(BlueprintCallable, Category = "Placement System")
	void Placement_Accept();

	/*
		applys rotation around Z axis to the placement process, this will be applyed only if called after calling  Placement_Start() function
		so please call it when you see the preview of the placement mesh

	*/
	UFUNCTION(BlueprintCallable, Category = "Placement System")
	void ApplyYawRotationToPlacement(TEnumAsByte<ERotationDirection> Direction);

	/*
		check the location under the cursor for classes of type ABuildingBase
		and and selectes it !
	*/
	bool CheckUnderCursorForSelection();

	/*
		selects currently hovered item (Of Type PlacementActor)
	*/
	UFUNCTION(BlueprintCallable, Category = "Placement System")
	void SelectUnderCursor();


	/*
		to start replacing an already placed/spawned building
	*/
	UFUNCTION(BlueprintCallable, Category = "Placement System")
	bool Replacement_Start(class ABuildingBase* building);

	/*
		to accept the replacement process and place the currently selected building in proper position
	*/
	UFUNCTION(BlueprintCallable, Category = "Placement System")
	void Replacement_Accept();

	/*
		to reject the replacement process and place the currently selected building to its original transform
	*/
	UFUNCTION(BlueprintCallable, Category = "Placement System")
	void Replacement_Cancel();

	/*
		removes the given building from the world
		and frees the reserved cells
	*/
	UFUNCTION(BlueprintCallable, Category = "Placement System")
	void RemoveBuilding(class ABuildingBase* toRemove);
		
};
