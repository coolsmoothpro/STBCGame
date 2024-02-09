// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#pragma region Print helpers


static void print(FString str, int printid = -1) {
	GEngine->AddOnScreenDebugMessage(printid,5 ,FColor::Blue , *str);
	UE_LOG(LogTemp , Warning , TEXT("%s") , *str);
}

static void print_Error(FString str, int printid = -1) {
	GEngine->AddOnScreenDebugMessage(printid, 5, FColor::Red, *str);
	UE_LOG(LogTemp, Error, TEXT("%s"), *str);
}

#pragma endregion