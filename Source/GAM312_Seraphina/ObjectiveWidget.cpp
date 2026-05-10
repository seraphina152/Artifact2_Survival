// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveWidget.h"

// Function to add materials to the current count
void UObjectiveWidget::AddMaterials(int materialsCollected)
{
	// Increases the amount of materials collected by the passed in value
	materialsCurrent += materialsCollected;

	// Checks if the player has completed the goal and sets it to true if they have
	if (materialsCurrent >= materialsGoal)
	{
		isMaterialComplete = true;
	}

	// Updates the objective text
	UpdateMaterialsObjective();
}

// Function to add built objects to the current count
void UObjectiveWidget::AddBuilds(int objectsBuilt)
{
	// Increases the amount of materials collected by the passed in value
	buildsCurrent += objectsBuilt;

	// Checks if the player has completed the goal and sets it to true if they have
	if (buildsCurrent >= buildsGoal)
	{
		isBuildComplete = true;
	}

	// Updates the objective text
	UpdateBuildObjective();
}
