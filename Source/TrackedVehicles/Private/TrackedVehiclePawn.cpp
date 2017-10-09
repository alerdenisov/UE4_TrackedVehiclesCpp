// Fill out your copyright notice in the Description page of Project Settings.

#include "TrackedVehiclePawn.h"

FName ATrackedVehiclePawn::MeshComponentName(TEXT("TrackedVehicleMesh"));
FName ATrackedVehiclePawn::MovementComponentName(TEXT("TrackedMovementComponent"));
FName ATrackedVehiclePawn::TracksBuilderComponentName(TEXT("TracksBuilderComponent"));

ATrackedVehiclePawn::ATrackedVehiclePawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(MeshComponentName);
    MovementComponent = CreateDefaultSubobject<UTrackedMovementComponent>(MovementComponentName);
    TracksBuilder = CreateDefaultSubobject<UTracksBuilderComponent>(TracksBuilderComponentName);
}