// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CBGPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CITYBUILDERGAME_API ACBGPlayerController : public APlayerController
{
	GENERATED_BODY()
	

	// Internal

	// references
	//  
	class APlayerPawn* _flyPawn_ref;
	class APawn* _FPSPawn_ref;
	class AGridManager* _grid_ref;



protected:

	virtual void OnPossess(APawn* pawn) override;
	virtual void SetupInputComponent() override;
	
	void SwitchCharacterMode();
	void SwitchToFlyMode();
	void SwitchToFPSMode();
	bool CanSwitchToFPSMode() const;


public:

	UPROPERTY(BlueprintReadWrite , EditAnywhere , Category = "City Builder Game | Classes")
	TSubclassOf<APawn> FPSCharacter_class;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "City Builder Game | switch Mode")
	float height_to_switch_to_FPS = 600;



	class AGridManager* GetGridManager() ;

};
