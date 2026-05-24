// Fill out your copyright notice in the Description page of Project Settings.
/*
Camera Use:
Camera libraries make using cameras in 3D games infinitely easier then they would be
with out them. Handling 3D cameras involves multiple vectors and matrices, such as the 
projection matrix, a transform vector for location, a rotation vector for the direction
the camera is looking, etc. Libraries make it so these things dont need to be done
by the programmer which makes getting started with 3D much easier than it used to be.

Linear Algebra:
Linear algebra is used everywhere in game development. As mentioned in the previous
section, cameras make extensive use of matrix math and vectors. Things like dot and cross
products are used to help with rotations and navigation. Most movement makes use of vector
math, both in 2D and 3D games. 

Trace/Collision:
Traces are one of the things that makes 3D games work. They allow you to check for collisions,
check if a certain object is in your path, retrieve information about other things around you,
etc. They can also be used in place of normal collision when you have objects moving at high
speeds. Collisions are what make most gameplay possible. Without them FPSes, platformers, 
adventure games, etc. wouldn't exist. There are many different types of collision tests you can 
do, but thankfully most modern engines have libraries to handle them. Simple collisions like
box or sphere collisions are pretty easy to implement, but having a library to handle more
complex collisions speeds up development a lot.

AI:
Navigation meshes make moving AI controlled objects around a map very easy. They are able to
determine, based on your settings, which areas of a level are traversable by the AI. From that
you can use the AI libraries to find paths to specific points or objects. Pathfinding algorithms 
can get a bit advanced, so having built-in libraries to handle it helps a lot. Pathfinding also
makes for more interesting AI enemies since you can set them on patrols or have them chase players
smartly, instead of just getting stuck on boxes while running in a straight line



*/
#pragma once

#include "Engine/Engine.h"
#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Resource_M.h"
#include "BuildingPart.h"
#include "PlayerHUDWidget.h"
#include "ObjectiveWidget.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerChar.generated.h"

UCLASS()
class GAM312_SERAPHINA_API APlayerChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerChar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Player Movement functions
	UFUNCTION()
	void MoveForward(float axisValue);

	UFUNCTION()
	void MoveRight(float axisValue);

	// Player Sprint functions
	UFUNCTION()
	void StartSprinting();

	UFUNCTION() 
	void StopSprinting();

	// Player Jump functions
	UFUNCTION()
	void StartJump();

	UFUNCTION()
	void StopJump();

	// Function to find objects in front of the player
	UFUNCTION()
	void FindObject();

	// Adds an amount of a specific resource to the player
	UFUNCTION()
	void GiveResources(float amount, EResourceEnum resourceType);

	// Blueprint event to update the crafting ui values
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateInventoryUI();

	// Player camera object
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* PlayerCamComp;

	// Player HUD widget
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPlayerHUDWidget* playerHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UObjectiveWidget* objectiveWidget;

	//************* Player stats ****************

	// Stat variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float CurrentHunger = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float MaxHunger = 100.0f;

	// Movement variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float WalkSpeed = 450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float RunSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float SprintCostPerSecond = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float StaminaPerSecond = 20.0f;

	// Amount of stamina it costs to gather resources
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float StaminaPerHit = 5.0f;

	// When the player runs out of stamina, this is the amount of seconds before
	// they start regenerating it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float OutOfStaminaTimer = 2.0f;

	// When the player uses stamina but isn't out, this is the amount of seconds
	// before they start regenerating it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float UsedStaminaTimer = 1.0f;

	// Used to track if the player is currently sprinting or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	bool IsSprinting = false;

	// Used to track is stamina regen is paused at the moment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	bool IsStaminaRegenPaused = false;
	
	// Timer used to keep stamina from regening when paused
	UPROPERTY()
	FTimerHandle StamTimerHandle;

	// Resource variables

	// Stores the player resource inventory in key-value array using an enum for the resource type
	// as the key, and an integer as the value ie how many of that resource the player has
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	TMap<EResourceEnum, int32> InventoryArray;

	// Reference to the decal that gets drawn when the player hits something
	UPROPERTY(EditAnywhere, Category = "HitMarker")
	UMaterialInterface* hitDecal;

	// Building variables

	// Boolean to track if the player is in the middle of placing a building part or not
	UPROPERTY()
	bool IsBuilding;

	// Boolean to track if the current object can be legally placed or not
	UPROPERTY()
	bool canPlaceObject;

	// Stores the player part inventory in key-value array using an enum for the part type
	// as the key, and an integer as the value ie how many of that part the player has
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building Supplies")
	TMap<EBuildingPartEnum, int32> BuildingInventoryArray;

	// Stores which child class of the building part the player is placing
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABuildingPart> BuildPartClass;

	// Stores the reference to the building part the player is in the process of placing
	UPROPERTY()
	ABuildingPart* spawnedPart;

	// Variables to handle the building grid snapping

	// This is the size of the grid we are snapping to
	UPROPERTY()
	float BuildingGridSize = 300.0f;

	// How far away to show the build piece. The player can change this with mouse wheel
	UPROPERTY()
	float BuildDistance = BuildingGridSize * 2.0f;

	// Minimum and maximum build distance to keep the object close to the player
	UPROPERTY()
	float MinimumBuildDistance = BuildingGridSize;

	UPROPERTY()
	float MaximumBuildDistance = BuildingGridSize * 5.0f;

	// Building functions
	
	// Crafts a specific building part and removes resources from the player to pay for it
	UFUNCTION(BlueprintCallable)
	void UpdateResources(float woodAmount, float stoneAmount, EBuildingPartEnum buildingObject);

	// Spawns in the building part for the player to place
	UFUNCTION(BlueprintCallable)
	void SpawnBuilding(EBuildingPartEnum buildingObject, bool& isSuccess);

	// Cancels the building process and deletes the spawned object
	UFUNCTION()
	void CancelBuilding();

	// Rotates the building part by 90 degrees
	UFUNCTION()
	void RotateBuilding();

	// Changes the build distance
	UFUNCTION()
	void IncreaseBuildDistance();
	UFUNCTION()
	void DecreaseBuildDistance();
	UFUNCTION()
	void ChangeBuildDistance(bool IncreaseDistance);

	// Stat functions
	UFUNCTION(BlueprintCallable)
	void SetHealth(float newHealth);

	UFUNCTION(BlueprintCallable)
	void ChangeHealth(float amount);

	UFUNCTION(BlueprintCallable)
	void SetStamina(float newStamina);

	UFUNCTION(BlueprintCallable)
	void ChangeStamina(float amount);

	UFUNCTION()
	void PauseStaminaRegen();

	UFUNCTION()
	void UnpauseStaminaRegen();

	UFUNCTION(BlueprintCallable)
	void SetHunger(float newHunger);

	UFUNCTION(BlueprintCallable)
	void ChangeHunger(float amount);

	UFUNCTION()
	void DecreaseHunger();

};
