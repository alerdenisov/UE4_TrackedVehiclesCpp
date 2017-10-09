// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "AI/Navigation/NavigationAvoidanceTypes.h"
#include "AI/RVOAvoidanceInterface.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/Actor.h"
#include "TrackedMovementComponent.generated.h"

// Should be a UENUM()? or only internal enuum?
enum ESuspensionSide {
    V_Left,
    V_Right
};

// TODO Make a simple vector and plain CPP structure
USTRUCT(BlueprintType)
struct FSuspensionInternalProcessing {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RootLoc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RootRot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Length = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Stiffness = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damping = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PreviousLenght = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SuspensionForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WheelCollisionLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector WheelCollisionNormal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Engaged;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<EPhysicalSurface> HitMaterial;


    FSuspensionInternalProcessing() {}

    static FSuspensionInternalProcessing Make(
        FVector rootLoc,
        FRotator rootRot,
        float length,
        float radius,
        float stiffness,
        float damping
    ) {
        FSuspensionInternalProcessing processing;
        {
            processing.RootLoc = rootLoc;
            processing.RootRot = rootRot;
            processing.Length = length;
            processing.Radius = radius;
            processing.Stiffness = stiffness;
            processing.Damping = damping;
        }

        return processing;
    }
};

USTRUCT(BlueprintType)
struct TRACKEDVEHICLES_API FSuspensionSetup {
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RootLoc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RootRot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaximumLenght = 23.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CollisionRadius = 34.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StiffnessForce = 4000000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DampingForce = 4000.0f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRACKEDVEHICLES_API UTrackedMovementComponent : public UPawnMovementComponent//, public IRVOAvoidanceInterface
{
	GENERATED_UCLASS_BODY()

public:
    /**Set the drive torque to be applied to a specific wheel*/
    UFUNCTION(BlueprintCallable, Category = "Inputs")
    void SetLeftTorque(float power);
    UFUNCTION(BlueprintCallable, Category = "Inputs")
    void SetRightTorque(float power);

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* WheelSweep;
	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* ThreadL;
	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* ThreadR;
	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* Body;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TrackMassKg = 600.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SprocketMassKg = 65.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SprocketRadiusCm = 24.05f;
	UPROPERTY(EditAnywhere, AdvancedDisplay)
		float AirDensity = 1.2922f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DragSurfaceArea = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DragCoef = 0.8f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<float> GearRatios;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DiferentialRatio = 3.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TransmissionEfficiency = 0.9f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Mu_X_Static = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Mu_Y_Static = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Mu_X_Kinetic = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Mu_Y_Kinetic = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RollingFrictionCoef = 0.02f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BrakeForce = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TreadLenght = 972.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> SplineCoordinatesR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> SplineCoordinatesL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FVector> SplineTangents;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TreadUVTiles;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TreadsOnSide;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float TreadHalfThickness;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSuspensionSetup> SuspesionSetupR;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FSuspensionSetup> SuspesionSetupL;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SleepVelocity = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SleepTimerSeconds = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool AutoGearBox = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GearUpShiftPrc = 0.9f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float GearDownShiftPrc = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float EngineExtraPowerRatio = 3.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool DebugMode = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        UCurveFloat* EngineTorqueCurve;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    virtual void UpdateThrottle();
    virtual void UpdateWheelsVelocity();
    virtual void UpdateAxleVelocity();
    virtual void UpdateEngineAndUpdateDrive();

    virtual void CalculateCollisions();
    virtual void CalculateCollisionForProcessor(TArray<FSuspensionInternalProcessing>& processors, int index, ESuspensionSide side);
    virtual bool TraceForSuspension(const FVector& start, const FVector& end, float radius, FHitResult& outResult);

    virtual void ApplyDriveForcesAndGetFrictionForcesOnSides();

    virtual void PrepareInputAxis();

protected:
    float DT;
    float RawLeftTorque;
    float RawRightTorque;

	UPROPERTY(Transient) float TrackTorqueTransferRight;
	UPROPERTY(Transient) float TrackTorqueTransferLeft;
	UPROPERTY(Transient) float WheelRightCoefficient;
	UPROPERTY(Transient) float WheelLeftCoefficient;
	UPROPERTY(Transient) float WheelForwardCoefficient;
	UPROPERTY(Transient) float SuspTargetVelocity;
	UPROPERTY(Transient) float TrackRightAngVel;
	UPROPERTY(Transient) float TrackLeftAngVel;
	UPROPERTY(Transient) float TrackRightLinVel;
	UPROPERTY(Transient) float TrackLeftLinVel;
	UPROPERTY(Transient) int CurrentGear;	
	UPROPERTY(Transient) float Throttle;
	UPROPERTY(Transient) float ThrottleIncrement;
	UPROPERTY(Transient) float TrackRightTorque;
	UPROPERTY(Transient) float TrackLeftTorque;
	UPROPERTY(Transient) float DriveRightTorque;
	UPROPERTY(Transient) float DriveLeftTorque;
	UPROPERTY(Transient) float MomentInertia;
	UPROPERTY(Transient) float DriveAxleTorque;
	UPROPERTY(Transient) float AxleAngVel;
	UPROPERTY(Transient) float BrakeRatioRight;
	UPROPERTY(Transient) float BrakeRatioLeft;
	UPROPERTY(Transient) FVector DriveRightForce;
	UPROPERTY(Transient) FVector DriveLeftForce;
	UPROPERTY(Transient) float TotalNumFrictionPoints;
	UPROPERTY(Transient) float TotalSupsForceLeft;
	UPROPERTY(Transient) bool ReverseGear;
	UPROPERTY(Transient) float TrackFrictionTorqueRight;
	UPROPERTY(Transient) float TrackFrictionTorqueLeft;
	UPROPERTY(Transient) float EngineRPM;
	UPROPERTY(Transient) float EngineTorque;
	UPROPERTY(Transient) float TrackRollingFrictionTorqueRight;
	UPROPERTY(Transient) float TrackRollingFrictionTorqueLeft;
	//UPROPERTY(Transient) UMaterialInstanceDynamic* TreadMaterialLeft;
	//UPROPERTY(Transient) UMaterialInstanceDynamic* TreadMaterialRight;
	UPROPERTY(Transient) float TreadUVOffsetRight;
	UPROPERTY(Transient) float TreadUVOffsetLeft;
	UPROPERTY(Transient) float TreadMeshOffsetRight;
	UPROPERTY(Transient) float TreadMeshOffsetLeft;
	UPROPERTY(Transient) float TreadsLastIndex;
	UPROPERTY(Transient) float SplineLengthAtConstruction;
	UPROPERTY(Transient) bool SleepMod;
	UPROPERTY(Transient) float SleepDelayTimer;
	UPROPERTY(Transient) float LastAutoGearBoxAxleCheck;
	UPROPERTY(Transient) int NeutralGearIndex;

	UPROPERTY(Transient)
        TArray<struct FSuspensionInternalProcessing> SuspensionsInternalRight;
	UPROPERTY(Transient)
        TArray<struct FSuspensionInternalProcessing> SuspensionsInternalLeft;
	UPROPERTY(Transient)
        TArray<UStaticMeshComponent*> SuspHandleRight;
	UPROPERTY(Transient)
        TArray<UStaticMeshComponent*> SuspHandleLeft;
	//UPROPERTY(Transient) UStaticMeshComponent* BackCarriageCoupler;


	virtual void ConstructSuspension();
};


FORCEINLINE float PrecalculateMomentOfInertia(float sprocketMassKg, float sprocketRadiusCm, float trackMassKg) {
	float squareRadius = sprocketRadiusCm * sprocketRadiusCm;
	float trackMassOnSquareRadius = trackMassKg * squareRadius;

	return ((sprocketMassKg * 0.5f) * squareRadius) + trackMassOnSquareRadius;
};