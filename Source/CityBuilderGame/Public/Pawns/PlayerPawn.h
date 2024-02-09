

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class CITYBUILDERGAME_API APlayerPawn : public APawn
{
	GENERATED_BODY()


	enum ETools CurrentTool;

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	class USceneComponent* root;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	class UCameraComponent* camera;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	class USpringArmComponent* cameraArm;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	class UPlacementComponent* PlacementComponent;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Components")
	class URoadPlacementComponent* RoadPlacementComponent;


	// variables
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings | Movement")
	float MovementSpeed = 5.0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings | Movement")
	float MovementSpeedMul = 1.0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings | Movement")
	float rotationSpeedSpeed = 5.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings | Movement")
	bool bApplyLocationLimitation = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings | Movement")
	FVector Min_MovementLimitation = FVector(0, 0, 400);
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings | Movement")
	FVector Max_MovementLimitation = FVector(1200, 600, 600);


	// whether the right mouse button 
	bool bIsRightButtonDown = false;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move_forward_backward(float v);
	void Move_right_left(float v);
	void Move_up_down(float v);
	void Mouse_X(float v);
	void Mouse_Y(float v);
	void Scrol_wheel(float v);

	void Right_Mouse_Btn_down();
	void Right_Mouse_Btn_up();

	void Left_Mouse_Btn_up();
	void Left_Mouse_Btn_Down();





	FVector ApplyLocationLimitation(FVector location);


public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;





	// Tools ManageMent

	UFUNCTION(BlueprintCallable)
	void ChangeCurrentTool(TEnumAsByte<enum ETools> tool);


};
