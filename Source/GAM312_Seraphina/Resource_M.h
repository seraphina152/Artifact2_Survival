// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Resource_M.generated.h"

// Enumerator for each resource type
UENUM(BlueprintType)
enum class EResourceEnum : uint8
{
	Wood UMETA(DisplayName = "Wood"),
	Stone UMETA(DisplayName = "Stone"),
	Berry UMETA(DisplayName = "Berry")
};

UCLASS()
class GAM312_SERAPHINA_API AResource_M : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AResource_M();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Amount of resources collected each hit of the node
	UPROPERTY(EditAnywhere)
	int resourceAmount = 5;

	// Total resources remaining in the node
	UPROPERTY(EditAnywhere)
	int totalResources = 100;

	// Enum of the resource type the node is using
	UPROPERTY(EditAnywhere)
	EResourceEnum resourceType = EResourceEnum::Wood;

	// Mesh component for the node
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

};


