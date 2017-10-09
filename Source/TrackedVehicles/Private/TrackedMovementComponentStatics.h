#pragma once

const float M2CM = 100.0f;
const float EPSILON = 0.000001f;

// fsign function to get sign part of numeric types
template <typename T> int fsign(T val)
{
    return (T(0) < val) - (val < T(0));
}

bool VTraceSphere(
        AActor* ActorToIgnore,
        const FVector& Start,
        const FVector& End,
        const float Radius,
        FHitResult& HitOut,
        ECollisionChannel TraceChannel=ECC_Pawn
) {
    FCollisionQueryParams TraceParams(FName(TEXT("VictoreCore Trace")), true, ActorToIgnore);
    TraceParams.bTraceComplex = true;
    TraceParams.bReturnPhysicalMaterial = true;

    //Ignore Actors
    TraceParams.AddIgnoredActor(ActorToIgnore);

    //Re-initialize hit info
//    HitOut = FHitResult(ForceInit);

    //Get World Source
    TObjectIterator<APlayerController> ThePC;
    if(!ThePC) return false;


    return ThePC->GetWorld()->SweepSingleByChannel(
            HitOut,         // struct FHitResult& OutHit,
            Start,          // const FVector& Start,
            End,            // const FVector& End,
            FQuat(),        // const FQuat& Rot,
            TraceChannel,   // ECollisionChannel TraceChannel,
            FCollisionShape::MakeSphere(Radius), // const FCollisionShape& CollisionShape,
            TraceParams);
//            const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam,
//            const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam) const;
}

FVector getUpVector(const FRotator& inRot)
{
    return FRotationMatrix(inRot).GetScaledAxis(EAxis::Z);
}
FVector getForwardVector(const FRotator& inRot)
{
    return FRotationMatrix(inRot).GetScaledAxis(EAxis::X);
}
FVector getRightVector(const FRotator& inRot)
{
    return FRotationMatrix(inRot).GetScaledAxis(EAxis::Y);
}

FVector transformDirection(const FTransform& transform, const FVector& direction)
{
    return transform.TransformVectorNoScale(direction);
}
FVector transformLocation(const FTransform& transform, const FVector& location)
{
    return transform.TransformPosition(location);
}

FVector inverseTransformDirection(const FTransform& transform, const FVector& direction)
{
    return transform.InverseTransformPosition(direction);
}
FVector inverseTransformLocation(const FTransform& transform, const FVector& location)
{
    return transform.InverseTransformPosition(location);
}

FVector projectVectorOnToVector(FVector V, FVector Target)
{
    if (Target.SizeSquared() > SMALL_NUMBER)
    {
        return V.ProjectOnTo(Target);
    }
    else
    {
        ReportError_ProjectVectorOnToVector();
        return FVector::ZeroVector;
    }
}

FVector projectVectorToPlane(FVector V, FVector PlaneNormal)
{
    return FVector::VectorPlaneProject(V, PlaneNormal);
}


void setupSuspensionHandlersArray(TArray<FSuspensionInternalProcessing>* dest, const FSuspensionSetup& setup, UStaticMeshComponent* handler) {
    FTransform handlerRelativeTransform = handler->GetRelativeTransform();
    // TODO: possible GC issue (fiqure out proper way to create USTRUCTs)
    dest->Add(FSuspensionInternalProcessing::Make(
            handlerRelativeTransform.GetLocation(), // RootLoc
            handlerRelativeTransform.Rotator(),		// RootRot
            setup.MaximumLenght,		// Lenght
            setup.CollisionRadius,		// Radius
            setup.StiffnessForce,		// Stiffness
            setup.DampingForce		    // Damping
//            0.0f,						// PreviousLenght
//            FVector(),					// SuspensionForce
//            FVector(),					// WheelCollisionLocation
//            FVector(),					// WheelCollisionNormal
//            false,						// Engaged
//            TEnumAsByte<EPhysicalSurface>(EPhysicalSurface::SurfaceType_Default) // HitMaterial
    ));
};

float calculateInertia(float angVel, float torque, float inertia, float dt) {
    return angVel + (torque / inertia) * dt;
}

float calculateBrake(float angVelIn, float brakeRatio, float brakeForce, float dt)
{
    float brakeCoef = brakeRatio * brakeForce * dt;
    if(fabs(angVelIn) > fabs(brakeCoef))
    {
        return angVelIn - (brakeCoef * fsign(angVelIn));
    }
    else
    {
        return angVelIn;
    }
}

float calculateEngineRPM(float angVel, float gearRation, float diferentialRatio)
{
    // TODO Move magic number to constants
    return (angVel * gearRation * diferentialRatio * 60) / (PI * 2);
}

float clampEngineRPM(float engineRPM, UCurveFloat* curve)
{
    check(curve);
    float minTime;
    float maxTime;
    curve->GetTimeRange(minTime, maxTime);
    return FMath::Clamp(engineRPM, minTime, maxTime);
}

float calculateEngineTorque(float engineRPM, UCurveFloat* curve)
{
    // get value from curve and convert from meters to cm
    check(curve);
    return curve->GetFloatValue(engineRPM) * M2CM;
}

VFector getVelocityAtLocation(const UMovementComponent* movementComponent, const FVector& location)
{
    // get vehicle actor
    AActor* actor = movementComponent->GetOwner();
    UPrimitiveComponent* body = movementComponent->UpdatedPrimitive;
    FTransform actorTransform = actor->GetTransform();

    FVector linearVelocity = body->GetPhysicsLinearVelocity(NAME_None);
    FVector angularVelocity = body->GetPhysicsAngularVelocity(NAME_None);
    FVector centerOfMass = body->GetCenterOfMass(NAME_None);

    FVector localLinearVelocity = inverseTransformDirection(actorTransform, linearVelocity);
    FVector localAngularVelocity = inverseTransformDirection(actorTransform, angularVelocity) * 0.0174532925; // To radians
    FVector localCenterOfMass = inverseTransformLocation(actorTransform, centerOfMass);
    FVector localLocation = inverseTransformLocation(actorTransform, location);

    FVector difference = localCenterOfMass - localLocation;
    FVector differenceAngularCross = FVector::CrossProduct(localAngularVelocity, difference);
    FVector sumLinearAndAngular = differenceAngularCross + localLinearVelocity;

    return transformDirection(sumLinearAndAngular);
}

FVector2D getMuFromFrictionElipse(FVector velocityDirection, FVector forwardVector, float muXStatic, float muYStatic, float muXKinetic, float muYKinetic)
{
    float dot = FVector::DotProduct(velocityDirection, forwardVector);
    float negateSqrtSquare = fsqrt(1 - dot * dot);
    return FVector2D(FVector2D(muXStatic * dot, muYStatic * negateSqrtSquare).Size(), FVector2D(muXKinetic* dot, muYKinetic* negateSqrtSquare).Size());
}