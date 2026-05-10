// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAM312_SERAPHINA_API UObjectiveWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Stores the current amount of collected resources
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int materialsCurrent = 0;

	// Stores the amount of resources required to win
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int materialsGoal = 500;

	// Stores the current amount of built objects
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int buildsCurrent = 0;

	// Stores the amount of built objects required to win
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int buildsGoal = 5;

	// booleans to track if the two objectives are complete or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isMaterialComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isBuildComplete = false;

	// Function to add materials to the current count
	UFUNCTION()
	void AddMaterials(int materialsCollected);

	// Function to add built objects to the current count
	UFUNCTION()
	void AddBuilds(int objectsBuilt);

	// Blueprint function to update the material objective text
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateMaterialsObjective();

	// Blueprint function to update the building objective text
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateBuildObjective();
};
