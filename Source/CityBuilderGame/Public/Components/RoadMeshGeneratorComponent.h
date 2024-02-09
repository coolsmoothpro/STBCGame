// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "proceduralMeshComponent.h"
#include "CityBuilderGame/CityBuilderGame.h"
#include "RoadMeshGeneratorComponent.generated.h"



UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CITYBUILDERGAME_API ARoadActor : public AActor
{
	GENERATED_BODY()

	UStaticMeshComponent* mesh;

	ARoadActor() {
		mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
		mesh->SetupAttachment(GetRootComponent());
	}


public:
	void SetMesh(UStaticMesh* _mesh) {
		if (!_mesh) { print_Error("Invalid Mesh !  ARoadActor::SetMesh"); }
		mesh->SetStaticMesh(_mesh); 
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CITYBUILDERGAME_API URoadMeshGeneratorComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere , Category = "Road | Meshes")
	UStaticMesh* Road_Mesh_Straight;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Road | Meshes")
	UStaticMesh* Road_Mesh_X;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Road | Meshes")
	UStaticMesh* Road_Mesh_T;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Road | Meshes")
	UStaticMesh* Road_Mesh_L;

	TArray<AActor*> spawnedActors;




	class AGridManager* GetGridManager();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	 void UpdateRoadMesh(TArray<struct FGridCell> roadcells);
	 void MeshAndRotationForRoadCell(FGridCell& cell, UStaticMesh*& meshRef , FTransform& roadTransform);
};
