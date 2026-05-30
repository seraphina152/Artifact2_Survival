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

	// Stores the normal material used for the building part
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* BaseMaterial;

	// Stores the material that is used when placing the part. This will be either
	// red or green depending on if it can be placed or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* BuildingMaterial;

	// Dynamic material instance used to change the color of the material 
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicBuildingMaterial;

	// Colors are stored here just to be easier to change if needed
	UPROPERTY()
	FLinearColor CanPlaceColor = FLinearColor(0, 1, 0, 1);

	UPROPERTY()
	FLinearColor CanNotPlaceColor = FLinearColor(1, 0, 0, 1);

	// Changes the meshes current material to the base material
	UFUNCTION(BlueprintCallable, Category = "Materials")
	void ApplyBaseMaterial();

	// Changes the meshes current material to the dynamic building material
	UFUNCTION(BlueprintCallable, Category = "Materials")
	void ApplyBuildingMaterial();

	// Updates the materials color based on whether the part can be placed or not
	UFUNCTION(BlueprintCallable, Category = "Materials")
	void UpdateBuildingMaterialColor(bool canPlace);

	// Stores the pivot arrow that the mesh rotates around
	UPROPERTY(EditAnywhere)
	UArrowComponent* PivotArrow;

	UPROPERTY(EditAnywhere)
	FRotator CurrentRotation = FRotator(0,0,0);
};
