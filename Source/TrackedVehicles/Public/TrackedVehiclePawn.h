// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TrackedMovementComponent.h"
#include "TracksBuilderComponent.h"
#include "TrackedVehiclePawn.generated.h"

UCLASS()
class TRACKEDVEHICLES_API ATrackedVehiclePawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:
	/// @brief Skeletal mesh component name
	/// @details Use it in ObjectInitialyser.DoNotCreateDefaultSuboject to prevent instatiate a mesh
	static FName MeshComponentName;

	/// @brief Movement component name
	/// @details Use it in ObjectInitialyser.DoNotCreateDefaultSuboject to prevent instatiate a component
	static FName MovementComponentName;

	/// @brief Tracks builder component name
	/// @details Use it in ObjectInitialyser.DoNotCreateDefaultSuboject to prevent instatiate a component
	static FName TracksBuilderComponentName;

	/// @brief Reference to primary skeletal mesh of a vehicle
	/// @details Skeletal mesh should have a wheels as a bones
	UPROPERTY(Category = "TrackedVehicle", BlueprintReadWrite, EditAnywhere)
    USkeletalMeshComponent* Mesh;

	/// @brief Reference to the movement component
	UPROPERTY(Category = "TrackedVehicle", BlueprintReadWrite, EditAnywhere)
	UTrackedMovementComponent* MovementComponent;

	/// @brief Reference to the tracks builder
	UPROPERTY(Category = "TrackedVehicle", BlueprintReadWrite, EditAnywhere)
	UTracksBuilderComponent* TracksBuilder;
};
