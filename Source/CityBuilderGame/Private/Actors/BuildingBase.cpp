

#include "Actors/BuildingBase.h"
#include "Components/BoxComponent.h"
#include "Components/PlacementComponent.h"
#include "Actors/GridManager.h"

// Sets default values
ABuildingBase::ABuildingBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!root) {
		root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
		SetRootComponent(root);
	}

	if (!collision) {
		collision = CreateDefaultSubobject<UBoxComponent>(FName("Collsion"));
		collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		collision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		collision->SetupAttachment(root);
	}

	if (!Mesh) {
		Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetupAttachment(root);
	}

}


void Helper_DrawCellGizmo_DrawCell(UWorld* world, FVector Cell_StartPoint, float cell_size) {
	if (!world)return;

	TArray<FVector> Points = { Cell_StartPoint ,
					   Cell_StartPoint + FVector(cell_size,0,0),
					   Cell_StartPoint + FVector(cell_size,cell_size,0),
					   Cell_StartPoint + FVector(0,cell_size,0), };

	DrawDebugLine(world, Points[0], Points[1], FColor::Green, 1, -1, 0U, 2);
	DrawDebugLine(world, Points[1], Points[2], FColor::Green, 1, -1, 0U, 2);
	DrawDebugLine(world, Points[2], Points[3], FColor::Green, 1, -1, 0U, 2);
	DrawDebugLine(world, Points[3], Points[0], FColor::Green, 1, -1, 0U, 2);


}

void ABuildingBase::DrawCellGizmo()
{
	// clear all drawn lines
	GetWorld()->Exec(GetWorld(), TEXT("FlushPersistentDebugLines"));



	for (size_t i = 0; i < width_Cell; i++) {
		for (size_t j = 0; j < depth_Cell; j++) {
			FVector startLoc = GetActorLocation() + FVector(cell_size * i, cell_size * j, 0) ;
			Helper_DrawCellGizmo_DrawCell(GetWorld(), startLoc, cell_size);
		}
	}
}

void ABuildingBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	DrawCellGizmo();
}

// Called when the game starts or when spawned
void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();

	currentScale = normalScale;

}

// Called every frame
void ABuildingBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// whether to animate hover
	if (bShouldScaleOnHover)
		AnimateHover(DeltaTime);

}

void ABuildingBase::OnPreviewPlacement()
{
	OnPreviewPlacement_BP();
}

void ABuildingBase::AnimateHover(float DeltaTime)
{
	if ((bIsHovered && !FMath::IsNearlyEqual(currentScale, hoveredScale, 0.005))
		||
		(!bIsHovered && !FMath::IsNearlyEqual(currentScale, normalScale, 0.005)))
	{
		currentScale += scalingSpeed * DeltaTime * (bIsHovered ? 1 : -1);

		// how to clamp ?

		if (hoveredScale > normalScale) {
			currentScale = FMath::Clamp(currentScale, normalScale, hoveredScale);
		}
		else {
			currentScale = FMath::Clamp(currentScale, hoveredScale, normalScale);
		}
		// set
		Mesh->SetWorldScale3D(FVector(currentScale));
	}
}

bool ABuildingBase::RegisterForReplacement()
{
	if (PlacementComponent && bIsSelected)
		return PlacementComponent->Replacement_Start(this);
	else return false;
}

void ABuildingBase::RemoveBuilding()
{
	PlacementComponent->RemoveBuilding(this);
}

void ABuildingBase::OnPlaced(UPlacementComponent* PlacementComponentRef)
{
	ensureMsgf(PlacementComponentRef, TEXT("Error ABuildingBase::OnPlaced PlacementSystem Component reference is Invalid !"));
	PlacementComponent = PlacementComponentRef;
	BuildingId = FGuid::NewGuid();
	bFirstTimeToPlace = false;
	OnPlaced_BP();
}

void ABuildingBase::OnSelected()
{
	if (bIsSelected) return;

	bIsSelected = true;
	OnSelected_BP();
}

void ABuildingBase::OnUnSelected()
{
	if (!bIsSelected) return;
	bIsSelected = false;
	OnUnSelected_BP();
}

void ABuildingBase::OnReplace_Started()
{
	if (!bIsSelected) return;
	OnReplace_Started_BP();
}

void ABuildingBase::OnReplaced()
{
	bIsSelected = false;
	OnReplaced_BP();
}

void ABuildingBase::OnHovered()
{
	if (bIsHovered) return;

	bIsHovered = true;
	OnHovered_BP();
}

void ABuildingBase::OnUnHovered()
{
	if (!bIsHovered) return;

	bIsHovered = false;
	OnUnHovered_BP();
}

void ABuildingBase::OnPlacementProcessValidation(float DeltaTime, bool bCanPlaceInCurrentLocation, bool bIsFirstTime)
{
	// do cpp implementation !

	// call bp
	OnPlacementProcessValidation_BP(DeltaTime, bCanPlaceInCurrentLocation, bIsFirstTime);
}

