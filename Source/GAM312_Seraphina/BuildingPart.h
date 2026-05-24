// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "BuildingPart.generated.h"

// Enumerator for each building part type
UENUM(BlueprintType)
enum class EBuildingPartEnum : uint8
{
	Wall UMETA(DisplayName = "Wall"),
	Floor UMETA(DisplayName = "Floor"),
	Ceiling UMETA(DisplayName = "Ceiling")
};

UCLASS()
class GAM312_SERAPHINA_API ABuildingPart : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingPart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Stores which type of building part the object is
	UPROPERTY(EditAnywhere)
	EBuildingPartEnum buildingPartType = EBuildingPartEnum::Wall;

	// Stores the mesh component
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	// Stores the pivot arrow that the mesh rotates around
	UPROPERTY(EditAnywhere)
	UArrowComponent* PivotArrow;

	UPROPERTY(EditAnywhere)
	FRotator CurrentRotation = FRotator(0,0,0);
};
