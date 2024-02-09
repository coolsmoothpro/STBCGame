// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/RoadIntersectionActor.h"
#include "CityBuilderGame/CityBuilderGame.h"

// Sets default values
ARoadIntersectionActor::ARoadIntersectionActor()
{

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>(FName("root"));
	SetRootComponent(root);

	if (!RoadIntersectionMesh) {
		RoadIntersectionMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Road Intersection mesh"));
		RoadIntersectionMesh->SetupAttachment(root);
	}

}

// Called when the game starts or when spawned
void ARoadIntersectionActor::BeginPlay()
{
	Super::BeginPlay();
}

bool ARoadIntersectionActor::GetConnectPointTransformNearLocation(FVector loc, FTransform& SocketTransform) const
{
	return GetConnectPointTransform(GetNearestConnectPointToLoc(loc), SocketTransform);
}

bool ARoadIntersectionActor::GetConnectPointTransform(FName ConnectPointSocketname, FTransform& SocketTransform) const
{
	SocketTransform = RoadIntersectionMesh->GetSocketTransform(ConnectPointSocketname);
	return RoadIntersectionMesh->GetSocketByName(ConnectPointSocketname) != nullptr;
}

FName ARoadIntersectionActor::GetNearestConnectPointToLoc(FVector loc) const
{
	float min = std::numeric_limits<double>::infinity();
	FName nearestSocketName;
	for (FName n : RoadIntersectionMesh->GetAllSocketNames()) {
		float distance = FVector::Distance(RoadIntersectionMesh->GetSocketLocation(n), loc);
		if (distance < min) {
			min = distance;
			nearestSocketName = n;
		}
	}
	return nearestSocketName;
}


