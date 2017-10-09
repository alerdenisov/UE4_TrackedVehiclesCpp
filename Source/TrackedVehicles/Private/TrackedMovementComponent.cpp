// Fill out your copyright notice in the Description page of Project Settings.
#include "TrackedMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "TrackedMovementComponentStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

UTrackedMovementComponent::UTrackedMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	TrackMassKg = 600.0f;
	SprocketMassKg = 65.0f;
	SprocketRadiusCm = 24.05f;

	// Air params
	AirDensity = 1.2922f;

	// Precalculate Moment of inertia based on mass and radius of suspension and mass of track
	MomentInertia = PrecalculateMomentOfInertia(SprocketMassKg, SprocketRadiusCm, TrackMassKg);

	// TODO: Register suspension handlers!
	this->ConstructSuspension();

	// TODO: Add center of mass visualization!
	// this->SetupVisualizationCenterOfMass();

	// TODO: Create spline materials
	// TODO: Build tracks spline
}

void UTrackedMovementComponent::SetLeftTorque(float power)
{
    RawLeftTorque = power;
}

void UTrackedMovementComponent::SetRightTorque(float power)
{
    RawRightTorque = power;
}

void UTrackedMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    TotalNumFrictionPoints = 0.0f;

    this->DT = DeltaTime;
    this->PrepareInputAxis();
	//   0: PutToSleep                                          TODO
    this->UpdateThrottle();
    this->UpdateWheelsVelocity();
    // 103: AnimateWheels
    // 104: AnimateThreadsMaterial
    // 105: AnimateThreadsSpline
    // 106: AnimateThreadsInstanciatedMesh
    this->UpdateAxleVelocity();
    this->UpdateEngineAndUpdateDrive();
    this->CalculateCollisions();
    this->ApplyDriveForcesAndGetFrictionForcesOnSides();
};

void UTrackedMovementComponent::PrepareInputAxis() {
    // Move?
    if(fabs(RawLeftTorque) > EPSILON && fabs(RawRightTorque) > EPSILON)
    {
        /*
         *  LEFT     RIGHT
         *  +1.0  +  -0.2
         *  -0.2  +  +1.0
         *  +0.5  +  -1.0
         *
         *
         *
         *
         */


        if((RawLeftTorque + RawRightTorque) > 0.0f)
        {
            // Forward

        }
        else
        {
            // Backward

        }
    }
    else
    {
        // Set full brake
    }

//    WheelRightCoefficient = LatestRightAxis;
//    WheelLeftCoefficient = -LatestRightAxis;//
//    float rightWeight = LatestRightAxis > 0 ? 1.0f : -1.0f;
//    float leftWeight  = LatestRightAxis < 0 ? 1.0f : -1.0f;
}

void UTrackedMovementComponent::UpdateThrottle()
{
    // TODO: Move magic numbers to constants
    TrackTorqueTransferRight = FMath::Clamp(WheelRightCoefficient + WheelForwardCoefficient, -1.0f, 2.0f);
    TrackTorqueTransferLeft = FMath::Clamp(WheelLeftCoefficient + WheelForwardCoefficient, -1.0f, 2.0f);


    // TODO: Epsilon compare
    if( fmax(fabs(TrackTorqueTransferRight), fabs(TrackTorqueTransferLeft)) != 0.0f )
    {
        ThrottleIncrement = 0.5f;
    }
    else
    {
        ThrottleIncrement = -1.0f;
    }

    Throttle = FMath::Clamp(Throttle + ThrottleIncrement * DT, 0.0f, 1.0f);
};

void UTrackedMovementComponent::UpdateWheelsVelocity()
{
    // TODO: Vector Math against float add

    // calculate torgues for tracks
    TrackRightTorque = DriveRightTorque + TrackFrictionTorqueRight + TrackRollingFrictionTorqueRight;
    TrackLeftTorque = DriveLeftTorque + TrackFrictionTorqueLeft + TrackRollingFrictionTorqueLeft;

    // apply brake coff to angular velocities
    float trackRightVelInertia = calculateInertia(TrackRightAngVel, TrackRightTorque, MomentInertia, DT);
    float trackLeftVelInertia = calculateInertia(TrackLeftAngVel, TrackLeftTorque, MomentInertia, DT);
    TrackRightAngVel = calculateBrake(trackRightVelInertia, BrakeRatioRight, BrakeForce, DT);
    TrackLeftAngVel = calculateBrake(trackLeftVelInertia, BrakeRatioLeft, BrakeForce, DT);

    TrackRightLinVel = TrackRightAngVel * SprocketRadiusCm;
    TrackLeftLinVel = TrackLeftAngVel * SprocketRadiusCm;
};

void UTrackedMovementComponent::UpdateAxleVelocity()
{
    AxleAngVel = (fabs(TrackRightAngVel) + fabs(TrackLeftAngVel)) / 2.0f;
}

void UTrackedMovementComponent::UpdateEngineAndUpdateDrive()
{
    // TODO: Calculate CurrentGearRatio
//    EngineRPM = clampEngineRPM(calculateEngineRPM(AxleAngVel, CurrentGearRatio, DiferentialRatio), *EngineTorqueCurve);
    EngineRPM = clampEngineRPM(calculateEngineRPM(AxleAngVel, 1.0f, DiferentialRatio), EngineTorqueCurve);
    EngineTorque = calculateEngineTorque(EngineRPM, EngineTorqueCurve) * Throttle;
}

void UTrackedMovementComponent::ConstructSuspension()
{
	for (int leftIndex = 0; leftIndex < SuspHandleLeft.Num(); leftIndex++)
    {
		setupSuspensionHandlersArray(&SuspensionsInternalLeft, SuspesionSetupL[leftIndex], SuspHandleLeft[leftIndex]);
	}

	for (int rightIndex = 0; rightIndex < SuspHandleRight.Num(); rightIndex++)
    {
		setupSuspensionHandlersArray(&SuspensionsInternalRight, SuspesionSetupR[rightIndex], SuspHandleRight[rightIndex]);
	}
}

void UTrackedMovementComponent::CalculateCollisions()
{
    // Right side
    for(int rIndex = 0; rIndex < SuspensionsInternalRight.Num(); rIndex++)
    {
        // Instance of suspension processor
        this->CalculateCollisionForProcessor(SuspensionsInternalRight, rIndex, ESuspensionSide::V_Right);
    }

    // Left side
    for(int lIndex = 0; lIndex < SuspensionsInternalLeft.Num(); lIndex++)
    {
        // Instance of suspension processor
        this->CalculateCollisionForProcessor(SuspensionsInternalLeft, lIndex, ESuspensionSide::V_Left);
    }
}

void UTrackedMovementComponent::CalculateCollisionForProcessor(TArray<FSuspensionInternalProcessing>& processors, int index, ESuspensionSide side)
{
    // get vehicle actor
    AActor* actor = this->GetOwner();

    // get vehicle transform
    FTransform actorTransform = actor->GetTransform();

    FSuspensionInternalProcessing suspensionProcessor = processors[index];

    // calculate suspension up vector (in world space)
    //FVector upVector = KismetMathLibrary::TransformDirection(actorTransform, KismetMathLibrary::GetUpVector(processors[index].RootRot));
    FVector upVector = transformDirection(actorTransform, getUpVector(suspensionProcessor.RootRot));
    FVector forwardVector = transformDirection(actorTransform, getForwardVector(suspensionProcessor.RootRot));
    FVector rightVector = transformDirection(actorTransform, getRightVector(suspensionProcessor.RootRot));
    FVector suspensionWorldLocation = transformLocation(actorTransform, suspensionProcessor.RootLoc);
    FVector suspensionEndLocation = suspensionWorldLocation + (upVector * -suspensionProcessor.Length);

    FHitResult hitResult = FHitResult(ForceInit);

    float wheelAngVelocity = side == ESuspensionSide::V_Left ? TrackLeftAngVel : TrackRightAngVel;
    float wheelLinVelocity = side == ESuspensionSide::V_Left ? TrackLeftLinVel : TrackRightLinVel;

    float suspensionNewLength = suspensionProcessor.Length;
    FVector suspensionForce = FVector();
    FVector impactPoint = FVector();
    FVector impactNormal = FVector();
    bool engaged = false;
    TEnumAsByte<EPhysicalSurface> hitMaterial = TEnumAsByte<EPhysicalSurface>(EPhysicalSurface::SurfaceType_Default);


    if(TraceForSuspension(suspensionWorldLocation, suspensionEndLocation, suspensionProcessor.Radius, hitResult))
    {
        suspensionNewLength = FVector::Distance(suspensionWorldLocation, hitResult.Location);
        impactPoint = hitResult.ImpactPoint;
        impactNormal = hitResult.ImpactNormal;
        engaged = true;

        if(hitResult.PhysMaterial.IsValid()) {
            UPhysicalMaterial* physicalMaterial = hitResult.PhysMaterial.Get();
            hitMaterial = physicalMaterial->SurfaceType;
        }


        // Calculate suspension force:

        // rate and stiffness
        float compressionRate = FMath::Clamp((suspensionProcessor.Length - suspensionNewLength) / suspensionProcessor.Length, 0.0f, 1.0f);
        float compressionStiffness = compressionRate * suspensionProcessor.Stiffness;
        // calculate susp velocity
        float suspensionVelocity = (suspensionNewLength - suspensionProcessor.PreviousLenght) / DT;
        suspensionVelocity = suspensionProcessor.Damping * (SuspTargetVelocity - suspensionVelocity);

        float suspensionForceMagnitude = compressionRate * compressionStiffness + suspensionVelocity;

        suspensionForce = upVector * suspensionForceMagnitude;

        // TODO: Add force to mesh!
        if(UpdatedPrimitive->IsSimulatingPhysics(NAME_None)) {
            UpdatedPrimitive->AddForceAtLocation(suspensionForce, suspensionWorldLocation, NAME_None);
        }
//        body.AddForceAtLocation(suspensionForce, suspensionWorldLocation);

        if(hitResult.Component.IsValid()) {
            UPrimitiveComponent* component = hitResult.Component.Get();
            if(component->IsSimulatingPhysics(NAME_None)) {
                component->AddForceAtLocation(-suspensionForce, impactPoint, NAME_None);
            }
        }

        TotalNumFrictionPoints++;
    }


    // Update Processor
    processors[index].PreviousLenght = suspensionNewLength;
    processors[index].SuspensionForce = suspensionForce;
    processors[index].WheelCollisionLocation = impactPoint;
    processors[index].WheelCollisionNormal = impactNormal;
    processors[index].Engaged = engaged;
    processors[index].HitMaterial = hitMaterial;
}


bool UTrackedMovementComponent::TraceForSuspension(const FVector& start, const FVector& end, float radius, FHitResult& outResult)
{
    return VTraceSphere(nullptr, start, end, radius, outResult);
}

void UTrackedMovementComponent::ApplyDriveForceAndGetFrictionForceOnSide(
        TArray<FSuspensionInternalProcessing>& processors,
        const FVector& driveForceSide,
        float trackLinVelSide)
{
    AActor* actor = movementComponent->GetOwner();
    FTransform actorTransform = actor->GetTransform();
    FVector forwardVector = getForwardVector(actorTransform.Rotator());
    FVector rightVector = getRightVector(actorTransform.Rotator());

    FVector linearForwardVelocity = forwardVector * trackLinVelSide;

    UPrimitiveComponent* primitiveComponent = UpdatedPrimitive;

    float bodyMass = primitiveComponent->GetMass();

    // Loop over suspension processors
    for(int processorIndex = 0; processorIndex < processors.Num(); processorIndex++)
    {
        FSuspensionInternalProcessing suspension = processors[processorIndex];
        // Only with engaged suspensions!
        if(suspension.Engaged)
        {
            float wheelLoadN = projectVectorOnToVector(suspension.SuspensionForce, suspension.WheelCollisionNormal).Size();
            FVector velocityAtPont = getVelocityAtLocation(this, suspension.WheelCollisionLocation);
            FVector relativeTrackVelocity = projectVectorToPlane(velocityAtPont - linearForwardVelocity, suspension.WheelCollisionNormal);

            FVector2D Mu = getMuFromFrictionElipse(relativeTrackVelocity.GetSafeNormal(), forwardVector, MuXStatic, MuYStatic, MuXKinetic, MuYKinetic);
            float muStatic = Mu.X;
            float muKinetic = Mu.Y;

            FVector velocityMultiplied = -relativeTrackVelocity * bodyMass / DT / TotalNumFrictionPoints;

            FVector projectedNormalizedForward = projectVectorToPlane(forwardVector, suspension.WheelCollisionNormal).Normalize();
            FVector projectedNormalizedRight   = projectVectorToPlane(rightVector,   suspension.WheelCollisionNormal).Normalize();

            FVector projectedVelocityOnForward = projectVectorOnToVector(velocityMultiplied, projectedNormalizedForward);
            FVector projectedVelocityOnRight   = projectVectorOnToVector(velocityMultiplied, projectedNormalizedRight);

            FVector fullStaticFrictionForce = projectedVelocityOnForward * MuXStatic + projectedVelocityOnRight * MuYStatic;
            FVector fullKineticFrictionForce = projectedVelocityOnForward * MuXKinetic + projectedVelocityOnRight * MuYKinetic;

            FVector projectedDriveForceSide = projectVectorToPlane(driveForceSide, suspension.WheelCollisionNormal);

            FVector fullStaticDriveForce = projectedDriveForceSide * MuXStatic;
            FVector fullKineticDriveForce = projectedDriveForceSide * MuXKinetic;

            // We want to apply higher friction if forces are bellow static friction limit
            if((fullStaticFrictionForce + fullStaticDriveForce).Size() >= wheelLoadN * muStatic) {

            }
        }
    }
}

void UTrackedMovementComponent::ApplyDriveForcesAndGetFrictionForcesOnSides()
{
    //Right side

    // Left side
}