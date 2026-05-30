// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPart.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ABuildingPart::ABuildingPart()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creates the building parts mesh and pivot arrow
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	PivotArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Pivot Arrow"));

	// Sets the pivot arrow as the root component and then attaches the mesh to it
	RootComponent = PivotArrow;
	Mesh->SetupAttachment(PivotArrow);
}

// Called when the game starts or when spawned
void ABuildingPart::BeginPlay()
{
	Super::BeginPlay();
	
	if (Mesh && BaseMaterial)
	{
		Mesh->SetMaterial(0, BaseMaterial);
	}

	if (BuildingMaterial)
	{
		DynamicBuildingMaterial = UMaterialInstanceDynamic::Create(BuildingMaterial, this);
	}
}

// Called every frame
void ABuildingPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Changes the meshes current material to the base material
void ABuildingPart::ApplyBaseMaterial()
{
	if (Mesh && BaseMaterial)
	{
		Mesh->SetMaterial(0, BaseMaterial);
	}
}

// Changes the meshes current material to the dynamic building material
void ABuildingPart::ApplyBuildingMaterial()
{
	if (Mesh && DynamicBuildingMaterial)
	{
		Mesh->SetMaterial(0, DynamicBuildingMaterial);
	}
}

// Updates the materials color based on whether the part can be placed or not
void ABuildingPart::UpdateBuildingMaterialColor(bool canPlace)
{
	if (DynamicBuildingMaterial)
	{
		if (canPlace)
		{
			DynamicBuildingMaterial->SetVectorParameterValue(TEXT("Color"), CanPlaceColor);
		}
		else
		{
			DynamicBuildingMaterial->SetVectorParameterValue(TEXT("Color"), CanNotPlaceColor);
		}
	}
}

