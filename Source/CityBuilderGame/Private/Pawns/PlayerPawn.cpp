

#include "pawns/PlayerPawn.h"
#include "camera/CameraComponent.h"
#include "Types/CBGTypes.h"
#include "Components/instancedStaticmeshComponent.h"
#include "Components/PlacementComponent.h"
#include "Components/RoadPlacementComponent.h"
#include "CityBuilderGame/CityBuilderGame.h"
#include "Gameframework/springarmComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!root) {
		root = CreateDefaultSubobject<USceneComponent>(FName("root"));
		SetRootComponent(root);
	}
	if (!cameraArm) {
		cameraArm = CreateDefaultSubobject<USpringArmComponent>(FName("Camera Arm"));
		cameraArm->TargetArmLength = 0;
		cameraArm->bUsePawnControlRotation = true;
		cameraArm->SetupAttachment(root);
	}

	if (!camera) {
		camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
		camera->SetupAttachment(cameraArm);
	}

	if (!RoadPlacementComponent) {
		RoadPlacementComponent = CreateDefaultSubobject<URoadPlacementComponent>(FName("Road Placement Component"));
	}

	if (!PlacementComponent) {
		PlacementComponent = CreateDefaultSubobject<UPlacementComponent>(FName("Placement Component"));
	}


}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();



	if (APlayerController* pc = Cast<APlayerController>(GetController())) {
		FInputModeGameAndUI mode = FInputModeGameAndUI();
		mode.SetHideCursorDuringCapture(false);
		mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		pc->SetInputMode(mode);
		pc->SetShowMouseCursor(true);
	}

}

void APlayerPawn::Move_forward_backward(float v)
{
	FVector loc = ApplyLocationLimitation(GetActorLocation() + camera->GetForwardVector() * v * MovementSpeed * MovementSpeedMul);
	SetActorLocation(loc);
}

void APlayerPawn::Move_right_left(float v)
{
	FVector loc = ApplyLocationLimitation(GetActorLocation() + camera->GetRightVector() * v * MovementSpeed * MovementSpeedMul);
	SetActorLocation(loc);

}

void APlayerPawn::Move_up_down(float v)
{
	FVector loc = ApplyLocationLimitation(GetActorLocation() + FVector::UpVector * v * MovementSpeed * MovementSpeedMul);
	SetActorLocation(loc);
}

void APlayerPawn::Mouse_X(float v)
{
	if (!bIsRightButtonDown) return;
	AddControllerYawInput(v);
}

void APlayerPawn::Mouse_Y(float v)
{
	if (!bIsRightButtonDown) return;
	AddControllerPitchInput(-v);
}

void APlayerPawn::Scrol_wheel(float v)
{
	MovementSpeedMul += v;
	MovementSpeedMul = FMath::Clamp(MovementSpeedMul, 0.5, 10);
}



void APlayerPawn::Right_Mouse_Btn_down()
{
	bIsRightButtonDown = true;

	switch (CurrentTool)
	{
	case ET_Select:
		break;
	case ET_Place_Building:
		if (PlacementComponent) {
			PlacementComponent->Placement_Cancel();
		}
		break;
	case ET_Replace:
		if (PlacementComponent) {
			PlacementComponent->Replacement_Cancel();
			if (APlayerController* pc = Cast<APlayerController>(GetController())) {
				FInputModeGameAndUI mode = FInputModeGameAndUI();
				mode.SetHideCursorDuringCapture(false);
				mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				pc->SetInputMode(mode);
				pc->SetShowMouseCursor(true);
			}
		}
		break;
	case ET_Place_Road:

		if (RoadPlacementComponent) {
			RoadPlacementComponent->RoadPlacement_Cancel();
		}
		break;
	default:
		break;
	}

}

void APlayerPawn::Right_Mouse_Btn_up()
{
	bIsRightButtonDown = false;
}

void APlayerPawn::Left_Mouse_Btn_Down()
{

	switch (CurrentTool)
	{
	case ET_Select:
		break;
	case ET_Place_Building:
		break;
	case ET_Replace:
		if (PlacementComponent) {
			PlacementComponent->SelectUnderCursor();
		}
		break;
	case ET_Place_Road:
		if (RoadPlacementComponent) {
			RoadPlacementComponent->RoadPlacement_Start();
		}
		break;
	default:
		break;
	}

}

void APlayerPawn::Left_Mouse_Btn_up()
{


	switch (CurrentTool)
	{
	case ET_Select:
		break;
	case ET_Place_Building:
		if (PlacementComponent) {
			PlacementComponent->Placement_Accept();
		}
		break;
	case ET_Replace:
		if (PlacementComponent) {
			PlacementComponent->Replacement_Accept();
			if (APlayerController* pc = Cast<APlayerController>(GetController())) {
				FInputModeGameAndUI mode = FInputModeGameAndUI();
				mode.SetHideCursorDuringCapture(false);
				mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				pc->SetInputMode(mode);
				pc->SetShowMouseCursor(true);
			}
		}
		break;
	case ET_Place_Road:

		if (RoadPlacementComponent) {
			RoadPlacementComponent->RoadPlacement_Accept();
		}

		break;
	default:
		break;
	}

}




// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("forward_back", this, &APlayerPawn::Move_forward_backward);
	PlayerInputComponent->BindAxis("right_left", this, &APlayerPawn::Move_right_left);
	PlayerInputComponent->BindAxis("up_down", this, &APlayerPawn::Move_up_down);
	PlayerInputComponent->BindAxis("mouseX", this, &APlayerPawn::Mouse_X);
	PlayerInputComponent->BindAxis("mouseY", this, &APlayerPawn::Mouse_Y);
	PlayerInputComponent->BindAxis("scrol_wheel", this, &APlayerPawn::Scrol_wheel);


	PlayerInputComponent->BindAction("right mouse btn", EInputEvent::IE_Pressed, this, &APlayerPawn::Right_Mouse_Btn_down);
	PlayerInputComponent->BindAction("right mouse btn", EInputEvent::IE_Released, this, &APlayerPawn::Right_Mouse_Btn_up);
	
	PlayerInputComponent->BindAction("left mouse btn", EInputEvent::IE_Pressed, this, &APlayerPawn::Left_Mouse_Btn_Down);
	PlayerInputComponent->BindAction("left mouse btn", EInputEvent::IE_Released, this, &APlayerPawn::Left_Mouse_Btn_up);


	



}

void APlayerPawn::ChangeCurrentTool(TEnumAsByte<ETools> tool)
{
	CurrentTool = tool;
}

FVector APlayerPawn::ApplyLocationLimitation(FVector location)
{
	if (!bApplyLocationLimitation) return location;

	FVector newloc;
	newloc.X = FMath::Clamp(location.X, Min_MovementLimitation.X, Max_MovementLimitation.X);
	newloc.Y = FMath::Clamp(location.Y, Min_MovementLimitation.Y, Max_MovementLimitation.Y);
	newloc.Z = FMath::Clamp(location.Z, Min_MovementLimitation.Z, Max_MovementLimitation.Z);

	return newloc;

}

