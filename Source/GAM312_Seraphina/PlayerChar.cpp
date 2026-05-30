// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChar.h"
#include <algorithm>

// Sets default values
APlayerChar::APlayerChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creates the camera and attaches it to the players head
	PlayerCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Cam"));
	PlayerCamComp->SetupAttachment(GetMesh(), "head");
	
	// Makes the camera rotate with the player since it's a first-person camera
	PlayerCamComp->bUsePawnControlRotation = true;

	// Adds the three resource types to the players inventory with a starting count of zero
	InventoryArray.Add(EResourceEnum::Wood, 0);
	InventoryArray.Add(EResourceEnum::Stone, 0);
	InventoryArray.Add(EResourceEnum::Berry, 0);

	// Adds the three building types to the players inventory with a starting count of zero
	BuildingInventoryArray.Add(EBuildingPartEnum::Wall, 0);
	BuildingInventoryArray.Add(EBuildingPartEnum::Floor, 0);
	BuildingInventoryArray.Add(EBuildingPartEnum::Ceiling, 0);

}

// Called when the game starts or when spawned
void APlayerChar::BeginPlay()
{
	Super::BeginPlay();

	// Timer to reduce hunger or health every 2 seconds
	FTimerHandle StatsTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(StatsTimerHandle, this, &APlayerChar::DecreaseHunger, 2.0f, true);
}

// Called every frame
void APlayerChar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Checks if the player is sprinting
	if (IsSprinting)
	{
		// If they are, reduce their stamina
		ChangeStamina((SprintCostPerSecond * DeltaTime) * -1);
		
		// If the player runs out of stamina, stop sprinting
		if (CurrentStamina <= 0)
		{
			IsSprinting = false;
			StopSprinting();
		}
	}
	else
	{
		// If the player isn't sprinting, then increase their stamina
		if (!IsStaminaRegenPaused)
		{
			ChangeStamina(StaminaPerSecond * DeltaTime);
		}
	}

	// Check if the player is in the process of placing a part
	if (IsBuilding)
	{
		// Check that the part object exists
		if (spawnedPart)
		{
			// Start off assuming we can't place the object
			CanPlaceObject = false;

			// Set the parts location based on where the player is looking
			FVector StartLocation = PlayerCamComp->GetComponentLocation();
			FVector Direction = PlayerCamComp->GetForwardVector() * BuildDistance;
			FVector EndLocation = StartLocation + Direction;

			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(spawnedPart);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnFaceIndex = true;

			// Performs a line trace based on the above settings
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 0.1f);

			// Set the spawned parts location before we do anything else. This way the piece will follow
			// the player's camera no matter what we are looking at
			spawnedPart->SetActorLocation(EndLocation);
			spawnedPart->SetActorRotation(GetActorRotation() + spawnedPart->CurrentRotation);

			// If the line trace from straight out of the camera hits an object then we can
			// start trying to figure out where to place the piece
			if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
			{
				// Try to cast what we hit to a building part
				ABuildingPart* HitPart = Cast<ABuildingPart>(HitResult.GetActor());

				// If we didn't hit a building part, figure out if we are placing a floor
				// and set it to be ok to place
				if (!HitPart)
				{
					if (spawnedPart->buildingPartType == EBuildingPartEnum::Floor)
					{
						spawnedPart->SetActorLocation(HitResult.Location);
						spawnedPart->SetActorRotation(GetActorRotation() + spawnedPart->CurrentRotation);
						CanPlaceObject = true;
					}

				}
				// If we did hit a building part, then we need to figure out where we hit it,
				// what we hit, and whether our current piece can be placed on it or not
				else
				{
					if (HitPart->buildingPartType == EBuildingPartEnum::Floor ||
						HitPart->buildingPartType == EBuildingPartEnum::Wall)
					{

						FVector hitNormal = HitResult.ImpactNormal;
			
						// If the hit normal is up, then we are pointing at the top of a floor piece
						if (hitNormal.Z > 0.9f && HitPart->buildingPartType == EBuildingPartEnum::Floor)
						{
							// Since we are snapping to the top of a floor piece, we need to know what the Z
							// value is. This way we can change the size of the floor piece and have it still
							// snap properly without having to change anything in the code
							FVector spawnLocation = HitPart->GetActorLocation();
							spawnLocation.Z = HitResult.Location.Z;
							spawnedPart->SetActorLocationAndRotation(spawnLocation, HitPart->GetActorRotation() + spawnedPart->CurrentRotation);
							CanPlaceObject = true;
						}
						// If the hit normal is not up, then we can assume we are aiming at the side of something
						else
						{
							// If we are aiming at a floor piece, and we are placing a floor, then we can snap to 
							// the edge of the floor
							if (HitPart->buildingPartType == EBuildingPartEnum::Floor)
							{
								if (spawnedPart->buildingPartType == EBuildingPartEnum::Floor)
								{
									spawnedPart->SetActorRotation(HitPart->GetActorRotation() + spawnedPart->CurrentRotation);

									spawnedPart->SetActorLocation(HitPart->GetActorLocation() + (hitNormal * BuildingGridSize));
									CanPlaceObject = true;
								}
							}

							// If we are hitting a wall and placing a ceiling, then we can snap to the walls location
							if (HitPart->buildingPartType == EBuildingPartEnum::Wall)
							{
								if (spawnedPart->buildingPartType == EBuildingPartEnum::Ceiling)
								{
									spawnedPart->SetActorLocationAndRotation(HitPart->GetActorLocation(), HitPart->GetActorRotation() + spawnedPart->CurrentRotation);
									CanPlaceObject = true;
								}
							}
						}
					}
				}
			}

			// Updates the building material color based on whether the part can be placed or not
			spawnedPart->UpdateBuildingMaterialColor(CanPlaceObject);
		}
	}
}

// Called to bind functionality to input
void APlayerChar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Binds the player input to specific functions
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerChar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerChar::MoveRight);
	// Binds the player input for sprinting
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerChar::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerChar::StopSprinting);
	// The mouse look bindings call the built-in pitch and yaw functions of the Pawn class
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerChar::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerChar::AddControllerYawInput);
	// Pressing and releasing the jump button calls the built in jump functions
	PlayerInputComponent->BindAction("JumpEvent", IE_Pressed, this, &APlayerChar::StartJump);
	PlayerInputComponent->BindAction("JumpEvent", IE_Released, this, &APlayerChar::StopJump);
	// Pressing the interact button calls the FindObject functions
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerChar::FindObject);
	// Right-clicking cancels the build process if the player is building anything
	PlayerInputComponent->BindAction("CancelAction", IE_Pressed, this, &APlayerChar::CancelBuilding);
	// Pressing the rotate part button calls the Rotate function
	PlayerInputComponent->BindAction("RotatePart", IE_Pressed, this, &APlayerChar::RotateBuilding);
	// Scrolling the mouse wheel moves the building parts away from or closer to the player
	PlayerInputComponent->BindAction("ScrollUp", IE_Pressed, this, &APlayerChar::IncreaseBuildDistance);
	PlayerInputComponent->BindAction("ScrollDown", IE_Pressed, this, &APlayerChar::DecreaseBuildDistance);


}

// Moves the player forward and backward
void APlayerChar::MoveForward(float axisValue)
{
	// Gets the current rotation of the player
	const FRotator Rotation = Controller->GetControlRotation();
	// Zeros out the pitch and roll so we only use the yaw of the character. This stops the player
	// from moving slowly or stopping when looking up or down
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Converts the modified rotation to a vector and then moves the player forward/backward
	FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);
	// FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, axisValue);

}

// Moves the player left and right
void APlayerChar::MoveRight(float axisValue)
{
	// Gets the current rotation of the player
	const FRotator Rotation = Controller->GetControlRotation();
	// Zeros out the pitch and roll so we only use the yaw of the character. This stops the player
	// from moving slowly or stopping when looking up or down
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Converts the modified rotation to a vector and then moves the player left/right
	FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);
	// FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, axisValue);

}

// Increases the player's walk speed if they try to start sprinting and have stamina
void APlayerChar::StartSprinting()
{
	if (CurrentStamina > 0)
	{
		IsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

// Stops the player from sprinting and sets their walk speed back to normal
void APlayerChar::StopSprinting()
{
	if (IsSprinting)
	{
		IsSprinting = false;
		PauseStaminaRegen();
	}

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

}

// Makes the player jump
void APlayerChar::StartJump()
{
	// This sets the built-in jump variable of the character movement component
	// to true, making the player jump
	bPressedJump = true;
}

// Ends the player's jump
void APlayerChar::StopJump()
{
	// This sets the built-in jump variable of the character movement component
	// to false, making the player stop jump
	bPressedJump = false;
}

// Crafts a specific building part and removes resources from the player to pay for it
void APlayerChar::UpdateResources(float woodAmount, float stoneAmount, EBuildingPartEnum buildingObject)
{
	// Checks if the player resource array contains the wood and stone keys to avoid
	// indexing errors
	if (InventoryArray.Contains(EResourceEnum::Wood) && InventoryArray.Contains(EResourceEnum::Stone))
	{
		// Checks that the player has enough wood and stone to craft the item
		if (woodAmount <= InventoryArray[EResourceEnum::Wood] &&
			stoneAmount <= InventoryArray[EResourceEnum::Stone])
		{
			// Reduces the players resources by the amount needed for the item
			InventoryArray[EResourceEnum::Wood] -= woodAmount;
			InventoryArray[EResourceEnum::Stone] -= stoneAmount;

			// Check if the building inventory has the objects key in it already
			// If it does, then increase it by 1. If it doesnt, then add it to the
			// inventory
			if (BuildingInventoryArray.Contains(buildingObject))
			{
				BuildingInventoryArray[buildingObject]++;
			}
			else
			{
				BuildingInventoryArray.Add(buildingObject, 1);
			}
		}
	}
}

// Creates the actual object in the world when they player tries to build something
void APlayerChar::SpawnBuilding(EBuildingPartEnum buildingObject, bool& isSuccess)
{
	if (IsBuilding)
	{
		spawnedPart->Destroy();
	}

	// Make sure that we arent already building something and that the object
	// exists in the building array
	if (BuildingInventoryArray.Contains(buildingObject))
	{
		// Check that the player has the desired part available
		if (BuildingInventoryArray[buildingObject] >= 1)
		{
			// Setting IsBuilding to true stops the player from being able to build more
			// and makes the object follow the mouse in the tick event
			IsBuilding = true;

			// Creates the spawn parameters for the player. We dont use any of them here yet
			// but this allows us to in the future if we want
			FActorSpawnParameters SpawnParams;

			// Calculates the items starting location and rotation
			FVector StartLocation = PlayerCamComp->GetComponentLocation();
			FVector Direction = PlayerCamComp->GetForwardVector() * 400.0f;
			FVector EndLocation = StartLocation + Direction;
			FRotator myRot(0, 0, 0);

			// Spawns the building part into the world and sets the material
			spawnedPart = GetWorld()->SpawnActor<ABuildingPart>(BuildPartClass, EndLocation, myRot, SpawnParams);
			spawnedPart->ApplyBuildingMaterial();
			CanPlaceObject = false;
			spawnedPart->UpdateBuildingMaterialColor(CanPlaceObject);

			// Updates the helper text to show the controls for placing building parts
			playerHUD->SetHelperText("LMB: Place object. RMB: Cancel Placement. Scroll Wheel: Adjust distance. E: Rotate object.");

			// isSuccess was based by reference so setting it to true here acts like a return command
			isSuccess = true;
		}
		// If the player doesnt have any of the items we return false
		else
		{
			isSuccess = false;
		}
	}
}

// Cancels the building process and deletes the spawned object
void APlayerChar::CancelBuilding()
{
	// Checks that we are actually in the middle of building
	if (IsBuilding)
	{
		// Sets our building flag to false, destroys the spawned item, and clears the helper text
		IsBuilding = false;
		spawnedPart->Destroy();
		playerHUD->SetHelperText("");
	}
}

// Rotates the building part 90 degrees if the player is currently building something
void APlayerChar::RotateBuilding()
{
	if (IsBuilding)
	{
		float curRotY = spawnedPart->CurrentRotation.Yaw;
		curRotY += 90;
		if (curRotY >= 360)
			curRotY = 0;

		spawnedPart->CurrentRotation = FRotator(0, curRotY, 0);
	}
}

// Attempts to increase the build distance
void APlayerChar::IncreaseBuildDistance()
{
	ChangeBuildDistance(true);
}

// Attempts to decrease the build distance
void APlayerChar::DecreaseBuildDistance()
{
	ChangeBuildDistance(false);
}

// Changes the build distance. Passing True increases the distance, false decreases it
void APlayerChar::ChangeBuildDistance(bool IncreaseDistance)
{
	float changeDirection = IncreaseDistance? 1.0f:-1.0f;

	// Changes the build distance and then clamps it between our minimum and maximum values
	BuildDistance += BuildingGridSize * changeDirection;
	BuildDistance = std::clamp(BuildDistance, MinimumBuildDistance, MaximumBuildDistance);
}

// Sets the players health to the specified value
void APlayerChar::SetHealth(float newHealth)
{
	CurrentHealth = std::clamp(newHealth, 0.0f, MaxHealth);
	playerHUD->UpdateStatBars(CurrentHealth, MaxHealth, CurrentHunger, MaxHunger, CurrentStamina, MaxStamina);
}

// Adds an amount to the player's health (accepts negatives to remove health)
void APlayerChar::ChangeHealth(float amount)
{
	CurrentHealth = std::clamp(CurrentHealth + amount, 0.0f, MaxHealth);
	playerHUD->UpdateStatBars(CurrentHealth, MaxHealth, CurrentHunger, MaxHunger, CurrentStamina, MaxStamina);
}

// Sets the players stamina to the specified value
void APlayerChar::SetStamina(float newStamina)
{
	CurrentStamina = std::clamp(newStamina, 0.0f, MaxStamina);
	playerHUD->UpdateStatBars(CurrentHealth, MaxHealth, CurrentHunger, MaxHunger, CurrentStamina, MaxStamina);
}

// Adds an amount to the player's stamina (accepts negatives to remove stamina)
void APlayerChar::ChangeStamina(float amount)
{
	CurrentStamina = std::clamp(CurrentStamina + amount, 0.0f, MaxStamina);
	playerHUD->UpdateStatBars(CurrentHealth, MaxHealth, CurrentHunger, MaxHunger, CurrentStamina, MaxStamina);

	if (CurrentStamina <= 0 or amount < 0)
	{
		PauseStaminaRegen();
	}

}

// Pauses the players stamina regen. This is used when the player uses stamina or when they run out
void APlayerChar::PauseStaminaRegen()
{
	// Sets the amount of time to pause based on whether the player is out of stamina or not
	float pauseTime = (CurrentStamina <= 0)? OutOfStaminaTimer:UsedStaminaTimer;
	
	IsStaminaRegenPaused = true;

	// Sets a timer based on the calculated pause time. If this function gets called multiple times,
	// such as the player using an action that then runs them out of stamina, then the timer will reset
	// itself to the last call it received
	GetWorld()->GetTimerManager().SetTimer(StamTimerHandle, this, &APlayerChar::UnpauseStaminaRegen, pauseTime, false);
}

void APlayerChar::UnpauseStaminaRegen()
{
	IsStaminaRegenPaused = false;
}

// Sets the players hunger to the specified value
void APlayerChar::SetHunger(float newHunger)
{
	CurrentHunger = std::clamp(newHunger, 0.0f, MaxHunger);
	playerHUD->UpdateStatBars(CurrentHealth, MaxHealth, CurrentHunger, MaxHunger, CurrentStamina, MaxStamina);
}

// Adds an amount to the player's hunger (accepts negatives to remove hunger)
void APlayerChar::ChangeHunger(float amount)
{
	CurrentHunger = std::clamp(CurrentHunger + amount, 0.0f, MaxHunger);
	playerHUD->UpdateStatBars(CurrentHealth, MaxHealth, CurrentHunger, MaxHunger, CurrentStamina, MaxStamina);
}

// Attempts to decrease the player's hunger when called
void APlayerChar::DecreaseHunger()
{
	// Subtracts from the player's hunger
	ChangeHunger(-1.0f);

	// If the player is out of hunger, start removing health
	if (CurrentHunger <= 0)
	{
		ChangeHealth(-3.0f);
	}
}

// Searches in front of the player for an object
void APlayerChar::FindObject()
{
	// Checks that we arent building anything before trying to gather
	if (!IsBuilding)
	{
		// Creates a hit result to store information about what the line trace hits, if anything
		FHitResult HitResult;

		// Calculated the start and end location of the trace based on the camera's 
		// location and direction
		FVector StartLocation = PlayerCamComp->GetComponentLocation();
		FVector Direction = PlayerCamComp->GetForwardVector() * 800.0f;
		FVector EndLocation = StartLocation + Direction;

		// Sets a few parameters to control the line trace including: ignoring the player
		// character, using complex collision detection, and returning the triangle on the
		// mesh that we hit
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnFaceIndex = true;

		// Performs a line trace based on the above settings
		if (GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams))
		{
			// Attempts to cast the object we hit to the resource class
			AResource_M* HitResource = Cast<AResource_M>(HitResult.GetActor());

			if (CurrentStamina >= StaminaPerHit)
			{
				// If HitResource is valid, then that means we hit a resource
				// If it's not valid, then the line trace hit something else instead or nothing at all
				if (HitResource)
				{
					// Pulls the name and resource value from the resource we hit
					FString hitName = *UEnum::GetDisplayValueAsText(HitResource->resourceType).ToString();
					int resourceValue = HitResource->resourceAmount;

					// If the total resources are more then the resource value then we give the player
					// the amount that the resource normally gives. If the total resources are more than
					// zero but less than the resource value, then we give the player what resources are left
					int giveAmount = 0;

					if (HitResource->totalResources >= resourceValue)
					{
						giveAmount = resourceValue;
					}
					else if (HitResource->totalResources > 0)
					{
						giveAmount = HitResource->totalResources;
					}

					// Check if the amount to give is greater than zero. This isn't really needed
					// expect to stop the debug messages from appearing when there are no resources 
					// to get for some reason
					if (giveAmount > 0)
					{
						// Gives the player the resources
						GiveResources(giveAmount, HitResource->resourceType);

						// Reduces the resources total amount remaining
						HitResource->totalResources = std::max(0, HitResource->totalResources - resourceValue);

						// Creates a message and displays it so the player knows they recieved something
						FString txt = "Gained " + FString::FromInt(giveAmount) + TEXT(" ") + hitName +
							TEXT(". ") + FString::FromInt(HitResource->totalResources) +
							TEXT(" remaining.");

						// Sends a message to the message log on the player HUD
						playerHUD->AddMessageToLog(txt);

						// Spawns a red decal to signal that the player successfully hit a resource
						UGameplayStatics::SpawnDecalAtLocation(GetWorld(), hitDecal, FVector(10.0f, 10.0f, 10.0f),
							HitResult.Location, FRotator(-90.0f, 0.0f, 0.0f), 2.0f);

						// Reduces the players stamina so they can't just gather forever
						ChangeStamina(-1 * (StaminaPerHit));
					}

					// If the resource node is now empty, destroy it
					if (HitResource->totalResources <= 0)
					{
						HitResource->Destroy();

						// Creates a message that the resource is out and displays it
						FString txt = hitName + TEXT(" resource depleted.");
						// Sends a message to the message log on the player HUD
						playerHUD->AddMessageToLog(txt);
					}
				}
			}
		}
	}
	// If we were building something, then left-clicking ends the building. Effectively placing 
	// the object permanently in the world since it will no longer follow the player camera
	else
	{
		// Only place the object if the location is valid
		if (CanPlaceObject)
		{
			IsBuilding = false;
			// Removes the item from the players inventory
			BuildingInventoryArray[spawnedPart->buildingPartType] -= 1;

			// Increases the build objective by one
			objectiveWidget->AddBuilds(1);

			// Sets the material of the part back to normal
			spawnedPart->ApplyBaseMaterial();

			// Checks if the player has anymore parts left of the current type
			// If they do then we spawn in another part, if they dont then we clear the helper text
			if (BuildingInventoryArray[spawnedPart->buildingPartType] > 0)
			{
				bool isSuccess = false;
				SpawnBuilding(spawnedPart->buildingPartType, isSuccess);
			}
			else
			{
				playerHUD->SetHelperText("");
			}
		}

	}
}

// Adds an amount of a specific resource to the player
void APlayerChar::GiveResources(float amount, EResourceEnum resourceType)
{
	// Checks if the inventory already contains a key for the resource
	// If it does, then we increase the quantity by the passed in amount
	// If it doesnt, then we add it to the array with the passed in amount
	if (InventoryArray.Contains(resourceType))
	{
		InventoryArray[resourceType] += amount;
	}
	else
	{
		InventoryArray.Add(resourceType, amount);
	}

	// Calls a blueprint function to update the crafting UI since our inventory has changed
	UpdateInventoryUI();

	// Increases the materials collected for the materials objective
	objectiveWidget->AddMaterials(amount);
}


