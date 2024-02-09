// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoadIntersectionActor.generated.h"


UENUM(BlueprintType)
enum ERoadInterSectionType {

	ERIT_X = 0  UMETA(displayName = "X"),
	ERIT_T      UMETA(displayName = "T"),
	ERIT_L      UMETA(displayName = "L")
};


UCLASS()
class CITYBUILDERGAME_API ARoadIntersectionActor : public AActor
{
	GENERATED_BODY()
protected:
	// Sets default values for this actor's properties
	ARoadIntersectionActor();
	UPROPERTY(BlueprintReadWrite, EditAnywhere )
	UStaticMeshComponent* RoadIntersectionMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Road Intersection")
	TEnumAsByte<ERoadInterSectionType> RoadIntersectionType;



	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
		returns transform for a connect point socket name that is nearest to @loc
	*/
	UFUNCTION(BlueprintCallable)
	bool GetConnectPointTransformNearLocation(FVector loc, FTransform& SocketTransform)const;
	
	/*
		returns true if the socket name was valid
		and the transform of the socket
	*/
	UFUNCTION(BlueprintCallable)
	bool GetConnectPointTransform(FName ConnectPointSocketname , FTransform& SocketTransform)const;
	
	/*
		returns nearest socket name of a connection point
		a connection point is a point in the inter section  that a road can connect  seamlessly
	*/
	UFUNCTION(BlueprintCallable)
	FName GetNearestConnectPointToLoc(FVector loc) const;
	

};
