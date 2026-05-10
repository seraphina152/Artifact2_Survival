// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

UPlayerHUDWidget::UPlayerHUDWidget(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	// Sets the size of our message box array to 5 since this is how many lines it can display
	MessageLog.SetNum(5);
}

// Adds a new string to the top of the message log
void UPlayerHUDWidget::AddMessageToLog(const FString& msg)
{
	// Loops thru the message array backwards
	for (int i = MessageLog.Num() - 1; i > 0; --i)
	{
		// Sets the message at the current index to the one from the index before it
		// This basically moves the messages down the list and removes the oldest message
		MessageLog[i] = MessageLog[i - 1];
	}

	// Set the first message in the array to the new message
	MessageLog[0] = msg;

	// Tells the widget blueprint to update the text box
	UpdateMessageLog();
}

// Sets the helper text string to a new value
void UPlayerHUDWidget::SetHelperText(const FString& msg)
{
	// Sets the string and then tells the widget blueprint to update the text box
	HelperText = msg;
	UpdateHelperText();
}

