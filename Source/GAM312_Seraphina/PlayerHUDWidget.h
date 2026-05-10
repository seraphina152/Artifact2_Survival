// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAM312_SERAPHINA_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer);

public:
	// Stores an array of strings to display in the message log
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> MessageLog;

	// Stores a string to display in the helper text box near the bottom middle of the screen
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString HelperText;

	// Blueprint function that updates the size of the stat progress bars
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateStatBars(float currentHP, float maxHP, float currentHunger, float maxHunger, float currentStam, float maxStam);

	// Blueprint function that adds the message log array to the message box
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMessageLog();

	// Adds a new string to the top of the message log
	UFUNCTION()
	void AddMessageToLog(const FString& msg);

	// Blueprint function that adds a helper text string to the helper text box
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHelperText();

	// Sets the helper text string to a new value
	UFUNCTION()
	void SetHelperText(const FString& msg);

};
