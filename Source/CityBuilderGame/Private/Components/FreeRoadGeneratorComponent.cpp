// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FreeRoadGeneratorComponent.h"
#include "Components/splinemeshcomponent.h"
#include "CityBuilderGame/CityBuilderGame.h"


UFreeRoadGeneratorComponent::UFreeRoadGeneratorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFreeRoadGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();
}

float tickcounter = 0;

void UFreeRoadGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bShouldUpdate) return;

	tickcounter += DeltaTime;
	if (tickcounter > .08f) {
		tickcounter = 0;
		UpdateRoadMesh();
	}
}




TArray<USceneComponent*> splineMeshes;

void UFreeRoadGeneratorComponent::UpdateRoadMesh()
{

	for (int i = 0; i < splineMeshes.Num(); i++) {
		if (splineMeshes[i]->IsValidLowLevel()) {
			splineMeshes[i]->DestroyComponent();
		}
	}

	splineMeshes.Empty();

	// spline meshes needed
	int32 numNeededSplineMeshs = (int32) GetSplineLength() / RoadMeshLength;
	float reminder = FMath::Fmod((float)GetSplineLength(), RoadMeshLength);

	// change the strtch of the mesh based on current number of meshes spawned (to be spawned)
	float FlexableRoadMeshLength = RoadMeshLength;

	if (reminder > RoadMeshLength/2 ) {
		numNeededSplineMeshs++;

		FlexableRoadMeshLength -= (RoadMeshLength - reminder) / (float)numNeededSplineMeshs;
	}
	else {
		FlexableRoadMeshLength += (reminder / (float)numNeededSplineMeshs);
	}

	for (int i = 0; i < numNeededSplineMeshs; i++) {
		USplineMeshComponent* sp = NewObject<USplineMeshComponent>(this, FName(FString("sp").Append(FString::FromInt(i))));
		sp->SetMobility(EComponentMobility::Movable);
		sp->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		sp->RegisterComponent();
		sp->SetStaticMesh(roadmesh);
		sp->SetForwardAxis(ESplineMeshAxis::X, 0);

		float startDistance = i * FlexableRoadMeshLength, endDistance = (i + 1) * FlexableRoadMeshLength;

		FVector start_loc, start_tangent;
		start_loc = GetLocationAtDistanceAlongSpline(startDistance, ESplineCoordinateSpace::World) + FVector(0, 0, 10) ;
		start_tangent = GetTangentAtDistanceAlongSpline(startDistance, ESplineCoordinateSpace::World);

		start_tangent = start_tangent.GetClampedToSize(0, FlexableRoadMeshLength);


		FVector end_loc, end_tangent;
		end_loc = GetLocationAtDistanceAlongSpline(endDistance, ESplineCoordinateSpace::World) + FVector(0, 0, 10);
		end_tangent = GetTangentAtDistanceAlongSpline(endDistance, ESplineCoordinateSpace::World);

		end_tangent = end_tangent.GetClampedToSize(0, FlexableRoadMeshLength);

		sp->SetStartAndEnd(start_loc, start_tangent, end_loc, end_tangent, 1);

		splineMeshes.Add(sp);

	}

}

void UFreeRoadGeneratorComponent::SetRoadMesh(UStaticMesh* inMesh)
{
	roadmesh = inMesh;
}

void UFreeRoadGeneratorComponent::OnStart(FVector start_loc)
{
	SetLocationAtSplinePoint(0,start_loc,ESplineCoordinateSpace::World,0);
}

void UFreeRoadGeneratorComponent::OnUpdate(FVector update_loc, FVector direction, bool bAutoDirection)
{

	if (bAutoDirection) {
		direction = GetLocationAtSplinePoint(GetLastSplineIndex(), ESplineCoordinateSpace::World) - GetLocationAtSplinePoint(GetLastSplineIndex() - 1, ESplineCoordinateSpace::World);
		direction.Normalize();
	}
	else {
		direction = direction * RoadMeshLength;
	}

	SetLocationAtSplinePoint(GetLastSplineIndex(),update_loc,ESplineCoordinateSpace::World,0);
	SetTangentsAtSplinePoint(GetLastSplineIndex(), -direction, direction, ESplineCoordinateSpace::World);

}

void UFreeRoadGeneratorComponent::AddPointAtLoc(FVector loc)
{
	FVector prev_point_loc = GetLocationAtSplinePoint(GetLastSplineIndex()-1, ESplineCoordinateSpace::World);


	if (FVector::Distance(prev_point_loc, loc) < RoadMeshLength) {
		print("Can not Spawn new Point the distance is not enough !");
		return;
	};


	// add spline point

	AddSplinePoint(loc ,ESplineCoordinateSpace::World,0);

}

void UFreeRoadGeneratorComponent::OnStop(FVector end_loc)
{
	bShouldUpdate = false;
}

void UFreeRoadGeneratorComponent::UpdatePointTangentScale(int32 pointIndex, float scaler)
{
	// there is only two points within the spline ! or this is the last or first index
	if (pointIndex == 0 || pointIndex == 1 && GetNumberOfSplinePoints() == 2 || pointIndex == GetNumberOfSplinePoints()-1) return;


	// find max vector length for tangent
	
	FVector prevPointLoc = GetLocationAtSplinePoint(pointIndex-1,ESplineCoordinateSpace::World);
	FVector SelfPointLoc = GetLocationAtSplinePoint(pointIndex , ESplineCoordinateSpace::World);
	FVector NextPointLoc = GetLocationAtSplinePoint(pointIndex+1, ESplineCoordinateSpace::World);

	float prevDistance = FVector::Distance(prevPointLoc,SelfPointLoc);
	float nextDistance = FVector::Distance(NextPointLoc, SelfPointLoc);
	
	float MaxTangentLength = prevDistance > nextDistance ? nextDistance : prevDistance;

	FVector l_tangent = GetLeaveTangentAtSplinePoint(pointIndex,ESplineCoordinateSpace::World);
	FVector a_tangent = GetArriveTangentAtSplinePoint(pointIndex, ESplineCoordinateSpace::World);

	l_tangent.Normalize();
	l_tangent *= (scaler * MaxTangentLength);
	
	a_tangent.Normalize();
	a_tangent *= (scaler * MaxTangentLength);


	print("Tangent : " + FString::SanitizeFloat(scaler * MaxTangentLength));
	print("scaler : " + FString::SanitizeFloat(scaler ));

	print("prev distance : " + FString::SanitizeFloat(prevDistance));
	print("next distance : " + FString::SanitizeFloat(nextDistance));




	SetTangentsAtSplinePoint(pointIndex, a_tangent, l_tangent , ESplineCoordinateSpace::World);
}

int32 UFreeRoadGeneratorComponent::GetLastSplineIndex() const
{
	return GetNumberOfSplinePoints()-1;
}
