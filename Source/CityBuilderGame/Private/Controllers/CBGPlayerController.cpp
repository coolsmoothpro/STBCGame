// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/CBGPlayerController.h"
#include "Pawns/PlayerPawn.h"
#include "CityBuilderGame/CityBuilderGame.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/GridManager.h"



void ACBGPlayerController::OnPossess(APawn* pawn)
{
	Super::OnPossess(pawn);

	if (APlayerPawn* p = Cast<APlayerPawn>(pawn)) {
		_flyPawn_ref = p;
	}
}

void ACBGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent) {
		InputComponent->BindAction("switch_character_mode" , EInputEvent::IE_Pressed , this , &ACBGPlayerController::SwitchCharacterMode);
	}
}

void ACBGPlayerController::SwitchCharacterMode()
{
	if (Cast<APlayerPawn>(GetPawn())) {
		// it is in fly mode. so switch to fps mode
		SwitchToFPSMode();
	}
	else {
		// switch to fly mode
		SwitchToFlyMode();
	}

}

void ACBGPlayerController::SwitchToFlyMode()
{

	if (Cast<APlayerPawn>(GetPawn())) return;

	if (_FPSPawn_ref) {
		_flyPawn_ref->SetActorTransform(_FPSPawn_ref->GetTransform());
	}
		
	UnPossess();
	Possess(_flyPawn_ref);

}

void ACBGPlayerController::SwitchToFPSMode()
{


	if (!CanSwitchToFPSMode() || !FPSCharacter_class ) return;
	

	if (!_FPSPawn_ref ) {
		// spawn fps character
		_FPSPawn_ref = GetWorld()->SpawnActor<APawn>(FPSCharacter_class);
		ensureMsgf(_FPSPawn_ref, TEXT("FPS pawn is invalid !  ACBGPlayerController::SwitchToFPSMode()"));
	}
	// reset location and rotation

	if (_flyPawn_ref) {
		_FPSPawn_ref->SetActorTransform(_flyPawn_ref->GetTransform());
	}

	UnPossess();
	Possess(_FPSPawn_ref);

}

bool ACBGPlayerController::CanSwitchToFPSMode() const
{
	if (!_flyPawn_ref) return false;

	FVector start = _flyPawn_ref->GetActorLocation(), end = start + FVector::DownVector * height_to_switch_to_FPS;
	FHitResult h;
	if (GetWorld()->LineTraceSingleByChannel(h , start , end ,ECC_Visibility)) {
		return true;
	}

	return false;
}

AGridManager* ACBGPlayerController::GetGridManager() 
{
	if (_grid_ref) return _grid_ref;
	_grid_ref = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	return _grid_ref; 
}
