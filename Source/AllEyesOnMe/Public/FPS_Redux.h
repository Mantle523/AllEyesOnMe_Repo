// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <vector>
#include "VectorTypes.h"
#include "Kismet/GameplayStatics.h"
#include "FPS_Redux.generated.h"

using namespace std;

#pragma region States

#pragma region Master State
//Player can be grounded, airborne or mantling
UENUM()
enum MotionStates { grounded, airborne, mantle };
#pragma endregion

#pragma region Ground States
//Player can be moving at a constant rate, accelerating or decelerating at the normal rate or decelerating slower after a long drop
UENUM()
enum GroundedStates { constantGround, accelerateGround, accelerateLongGround, decelerateGround };
#pragma endregion

#pragma region Aerial States
//Player decelerates based on "air resistance" to a given point, then moves at a constant, lessened rate
UENUM()
enum AirborneStates { constantAir, decelerateAir };
#pragma endregion

#pragma region Cover and Crouch States
//Player can be at full height, fully crouched, or transitioning either automatically or on command. Sprinting raises the player to full height.
UENUM()
enum CoverStates { FullHeight, FullCrouch, DuckingCover, RisingCover, DuckingCrouch, RisingCrouch, RisingSprint };
#pragma endregion

#pragma region Sprint States
//Player can be moving at normal rate, sprinting at max rate or accelerating between the two
UENUM()
enum SprintStates { disabledSprint, constantSprint, accelerateSprint, decelerateSprint };
#pragma endregion

#pragma region Mantle States
//"mantlingMantle" Good naming convention, there.
UENUM()
enum MantleStates { noneMantle, checkingMantle, mantlingMantle};
#pragma endregion

#pragma endregion

UCLASS()
class ALLEYESONME_API AFPS_Redux : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFPS_Redux();

#pragma region Variable Modification
	UFUNCTION(BlueprintCallable, Category = "MotionVariables")
		void OverrideGroundVariables(const float baseSpeedOverride, const float accTimeOverride, const float accLongTimeOverrive, const float decTimeOverride, const float graceTimeOverride);

	UFUNCTION(BlueprintCallable, Category = "MotionVariables")
		void OverrideAerialVariables(const float decTimeOverride, const float terminalInputMultOverride, const float initialInputMultOverride);

	UFUNCTION(BlueprintCallable, Category = "MotionVariables")
		void OverrideCoverVariables(const float heightDiffOverride, const float crouchTimeOverride, const float uncrouchTimeOverride, const float blendTimeOverride, const float crouchMultOverride);

	UFUNCTION(BlueprintCallable, Category = "MotionVariables")
		void OverrideAimVariables(const float aimSpeedOverride);

	UFUNCTION(BlueprintCallable, Category = "MotionVariables")
		void OverrideSprintVariables(const float sprintMultOverride, const float sprintTimeOverride, const float sprintDecTimeOverride, const float fovChangeOverride);

	UFUNCTION(BlueprintCallable, Category = "MotionVariables")
		void OverrideDetectionVariables(const float initDetectRateOverride);

	UFUNCTION(BlueprintCallable, Category = "MotionVariables")
		void OverridePhysicalTraits(const float groundDistanceOverride, const float mantleRangeOverride, const float coverRadiusOverride);

	UFUNCTION(BlueprintCallable, Category = "MotionVariables")
		void CrouchOriginPosition(const float playerHeadHeight);

	UFUNCTION(BlueprintCallable, Category = "MotionVariables")
		void FovOriginAngle(const float cameraFov);
#pragma endregion

#pragma region Input voids and variables
	//Ground Detection
	UFUNCTION(BlueprintCallable, Category = "InputHandling")
		void GroundDetection(const bool grounded);

	//Sets deltaTime on every frame, from blueprint
	UFUNCTION(BlueprintCallable, Category = "InputHandling")
		void TimeDifference(const float tempDelta);

	//Sets player motion and aim
	UFUNCTION(BlueprintCallable, Category = "InputHandling")
		void ControlInputs(const FVector2D motionInput, const FVector2D aimInput);

	//Sets whether or not the player is currently using a controller
	UFUNCTION(BlueprintCallable, Category = "InputHandling")
		void InputType(const bool typeController);

	//Sets camera facing after calculation
	UFUNCTION(BlueprintCallable, Category = "InputHandling")
		void PerspectiveInput(const FVector tempForward, const FVector tempRight);

	//Returns whether or not the player is currently using a controller
	UFUNCTION(BlueprintPure, Category = "InputHandling")
		FORCEINLINE bool InputTypeController() const { return inputTypeController; };

	//Sets crouch state
	UFUNCTION(BlueprintCallable, Category = "InputHandling")
		void CrouchInput();

	//Sets last frame direction
	UFUNCTION(BlueprintCallable, Category = "InputHandling")
		void RecordDirection(const FVector tempDirection);

#pragma endregion

#pragma region Outputs
	//Outputs player head vertical position
	UFUNCTION(BlueprintCallable, Category = "Output")
		FORCEINLINE float CrouchOutput() const;

	UFUNCTION(BlueprintPure, Category = "Output")
		FORCEINLINE float ReturnXMultiplier() const
	{
		return (currentXYMult.X);
	}

	UFUNCTION(BlueprintPure, Category = "Output")
		FORCEINLINE float ReturnYMultiplier() const
	{
		return (currentXYMult.Y);
	}

	UFUNCTION(BlueprintPure, Category = "Output")
		FORCEINLINE float ReturnTopSpeed() const;
#pragma endregion

#pragma region Automated Character Behaviour
	UFUNCTION(BlueprintCallable, Category = "Automated Inputs")
		void CrouchAdd();

	UFUNCTION(BlueprintCallable, Category = "Automated Inputs")
		void CrouchSubtract();

	UFUNCTION(BlueprintCallable, Category = "Automated Inputs")
		void CrouchAutomatic();

	UFUNCTION(BlueprintCallable, Category = "Automated Inputs")
		void UncrouchAutomatic();

	//Disables mantling effect
	UFUNCTION(BlueprintCallable, Category = "InputHandling")
		void DisableMantle();
#pragma endregion

#pragma region Debug
	UFUNCTION(BlueprintCallable, Category = "Debug Feature")
		FORCEINLINE FString CurrentCrouchState() const;

	UFUNCTION(BlueprintCallable, Category = "Debug Feature")
		FORCEINLINE FString CurrentForcedCover() const;

	UFUNCTION(BlueprintCallable, Category = "Debug Feature")
		FORCEINLINE FString CurrentForcedCoverExit() const;

	UFUNCTION(BlueprintCallable, Category = "Debug Feature")
		FORCEINLINE bool IsDecelerating() const;

	UFUNCTION(BlueprintCallable, Category = "Debug Feature")
		FORCEINLINE FVector2D currentAim() const;

	UFUNCTION(BlueprintCallable, Category = "DebugFeature")
		FORCEINLINE float CrouchTimer() const;

	UFUNCTION(BlueprintCallable, Category = "DebugFeature")
		FORCEINLINE float OutputChecker() const;
#pragma endregion

#pragma region Per-Frame Calculations

#pragma region Called From Blueprint
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void OnTickMantle(const FVector playerFacing);

	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void OnTickAimSmoothing();

	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void OnTickMotion();

	//Returns fullMotion minus its y component
	UFUNCTION(BlueprintPure, Category = "On-Tick")
		FORCEINLINE FVector lastDirection() const
	{
		return (anchorMotion);
	}

	//Returns tolerance for angular difference
	UFUNCTION(BlueprintPure, Category = "On-Tick")
		FORCEINLINE float returnTolerance() const
	{
		return (maxMantleAngle);
	}

	//Outputs directional input
	UFUNCTION(BlueprintPure, Category = "On-Tick")
		FORCEINLINE FVector2D MotionOutput() const;

	//Outputs airborne state
	UFUNCTION(BlueprintPure, Category = "On-Tick")
		FORCEINLINE bool AirborneState() const;

#pragma region Mantle Behaviour
	//Resets player mantling state
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void ResetMantle();

	//Returns if the player is currently mantling
	UFUNCTION(BlueprintPure, Category = "On-Tick")
		FORCEINLINE bool IsMantling() const
	{
		if (currentMotionState == mantle)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//Returns float progress through mantling behaviour
	UFUNCTION(BlueprintPure, Category = "On-Tick")
		FORCEINLINE float MantleProgress() const
	{
		return (smoothingMantleProgress + deltaTime) / mantleMax;
	}

	//Sets Y displacement during mantling
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void MantleDisplacement(const float progression);
#pragma endregion

#pragma region Crouch Behaviour
	//Determines whether the player is crouching
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		FORCEINLINE bool IsCrouching() const
	{
		if (currentCoverState != FullHeight && currentCoverState != FullCrouch)
		{
			return (true);
		}
		else
		{
			return (false);
		}
	}

	//Returns float progress through crouch behaviour
	UFUNCTION(BlueprintPure, Category = "On-Tick")
		FORCEINLINE float CrouchProgress() const
	{
		return ((smoothingCrouchProgress + deltaTime) / crouchMax);
	}

	//Returns float progress through uncrouch behaviour
	UFUNCTION(BlueprintPure, Category = "On-Tick")
		FORCEINLINE float UncrouchProgress() const
	{
		return ((smoothingCrouchProgress + deltaTime) / uncrouchMax);
	}

	//Returns true if the player is crouching, false if not
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		FORCEINLINE bool CrouchType() const
	{
		if (currentCoverState == DuckingCrouch || currentCoverState == DuckingCover)
		{
			return (true);
		}
		else
		{
			return (false);
		}
	}

	//Forces uncrouch state while mantling, airborne or sprinting
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void UncrouchForced();

	//Sets head position multiplier for crouching
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void CrouchDisplacement(const float position);
#pragma endregion

#pragma region Sprint Behaviour 
	//Determines whether the player is sprinting
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		FORCEINLINE bool IsSprinting() const;

	//Returns float progress through acceleration
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		FORCEINLINE float AccelerateSprintProgress() const
	{
		return ((smoothingSprintProgress + deltaTime) / sprintMax);
	}

	//Returns float progress through deceleration
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		FORCEINLINE float DecelerateSprintProgress() const
	{
		return ((smoothingSprintProgress + deltaTime) / sprintDecMax);
	}

	//Determines whether the player is accelerating or decelerating
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		FORCEINLINE bool IsAccelerating() const
	{
		if (currentSprintState == accelerateSprint)
		{
			return (true);
		}
		else
		{
			return (false);
		}
	}

	//Sprint inputs
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void SprintActivation();

	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void SprintDectivation();

	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		void SprintDisplacement(const float position);

	//Return FoV
	UFUNCTION(BlueprintCallable, Category = "On-Tick")
		float FovOutput() const
	{
		return (currentFoV);
	}
#pragma endregion

#pragma endregion

#pragma region Internal Enumeration States

#pragma region Motion Smoothing
	UFUNCTION()
		void Acceleration();

	UFUNCTION()
		void Grounded();

	UFUNCTION()
		void Airborne();
#pragma endregion

#pragma region Top Speed Calculations
	UFUNCTION()
		void CrouchedSpeed();

	UFUNCTION()
		void SprintingSpeed();
#pragma endregion

#pragma endregion

#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

#pragma region Inputs
	bool inputTypeController{ true };		//If true, the player last registered an input via gamepad
	float deltaTime{ 0.0f };			//Time since the last frame
	FVector cameraForward;				//Used for inputs and processing
	FVector cameraRight;

	FVector2D motion{ 0.0f, 0.0f };	//Current direction of player motion, relative to camera forward
	FVector2D aim{ 0.0f, 0.0f };		//Change in mouse position/controller axis from last frame
#pragma endregion

#pragma region Airborne check
	bool isAirborne = false;
#pragma endregion

#pragma region Motion State Variables

#pragma region Master State Variables
	TEnumAsByte<MotionStates> currentMotionState{ grounded };	//Under what conditions is the player currently moving?
	TEnumAsByte<MotionStates> lastMotionState{ grounded };		//What were the last conditions the player was moving under?
	float motionMagnitude{ 0.0f };								//Current player speed, reused during motion calculations
	FVector motionMagnitudeVector{ 0.0f, 0.0f, 0.0f };			//The above, when axis need independent scaling
	float maxSpeedCurrent{ 0.0f };								//Current top speed multiplier based on top speed, acceleration, sprint multiplier and crouch multiplier
#pragma endregion

#pragma region Grounded Variables
	TEnumAsByte<GroundedStates> currentGroundState{ constantGround }; //Current grounded motion state
	FVector2D velocity{ 0.0f, 0.0f };	//Player velocity on the current frame
	FVector2D GroundVelocityLast;	//Player's last recorded velocity while on the ground (y omitted)
	FVector2D groundVelocityHalt;   //Player's last recorded top speed while performing inputs, used for deceleration (y omitted)

	float topSpeed{ 8 };		//Player's top speed in m/s (Thermodynamic's top speed is 12, but this is STILL fast for a human
	float groundMax{ 0.6f };	//Time taken to reach top speed
	float groundLongMax{ 0.8f };//Time taken to reach top speed after a long fall
	float stopMax{ 0.2f };		//Time taken to decelerate from top speed to 0
	float graceMax{ 0.04f };	//Length of grace period between releasing controls and experiencing deceleration. Prevents deceleration when crossing axes on a gamepad

	float smoothingGroundProgress{ 0.0f };	//Progress through states/towards state changes on the ground
	float grace{ 0.0f };		//Progress through grace period between releasing controls and experiencing deceleration
	float hardLandingMult{ 1.0f };	//Current input multiplier from experiencing a hard landing
	float speedMult{ 1.0f };		//Player's current speed multiplier for crouched/sprinting behaviour
#pragma endregion

#pragma region Aerial Variables
	TEnumAsByte<AirborneStates> currentAirState = { constantAir };
	float AirVelocityLast;		//Straight-line aerial velocity on the last frame (used for momentum/inertia)
	float AirVelocityCurrent;   //Straight-line aerial velocity on the current frame (used for speed/acting force)

	float airMax{ 0.6f };				//Total airborne time for momentum to decay. This is half the time from Thermodynamic.
	float decayMax{ 0.0f };				//Total momentum retained once fully decayed. This value should be very small (0-0.1)
	float airMult{ 0.3f };				//Multiplier for directional input while airborne. Thermodynamic uses 0.6, but Eilidh does not have thrusters.

	float smoothingAirProgress{ 0.0f };		//Progress through states/toward state changes in the air
	bool hardLanding = false;	//Has the player been falling long enough to lose ALL their momentum? That's gonna be fun on the knees - triggers slow acceleration when back on the ground.
#pragma endregion

#pragma region Cover Variables
	TEnumAsByte<CoverStates> currentCoverState{ FullHeight };
	TEnumAsByte<CoverStates> lastCoverState{ FullHeight };
	float cameraHeightLast;				//Local position of the camera on the last frame
	float lastSmoothingHeight{ 0.0f };	//How far the player's y component was displaced before the last action was cancelled
	int crouchCounter{ 0 };				//How many cover objects are nearby?
	bool theFear{ true };				//Perfectly self-explanitory
	FString coverName;					//Debug

	float heightMin{ -40.0f };			//Minimum camera height relative to the camera parent object (does not move collisions)
	float playerHeadPos{ 0.0f };		//Height of player's head when not sprinting
	float playerHeadPosCurrent{ 0.0f };	//Current offset for the player's head position
	float crouchMax{ 0.6f };			//Total time taken to crouch
	float uncrouchMax{ 1.2f };			//Total time taken to uncrouch
	float sprintUncrouchMax{ 0.4f };	//Total time taken to uncrouch while sprinting/mantling
	float blendMax{ 0.3f };				//Total time taken to blend between ducking and rising.
	float crouchMult{ 0.8f };			//Speed mult while fully crouched
	float crouchMultCurrent{ 1.0f };	//Speed mult at current
	float crouchDetectionMult{ 0.6f };	//How much slower does detection initially build

	float smoothingCrouchProgress{ 0.0f };	//Progress through states/towards state changes while crouching
	float offsetAmp{ 0.0f };				//% progress to/from max offset
	float lastOffsetAmp{ 0.0f };			//% progress to/from max offset when last action was cancelled 
	bool crossTransition{ false };			//Has the player gone straight from one transition to the other?
	float blendProgress{ 0.0f };		//Smooths the transition when moving directly between ducking and rising (or vice-versa)
#pragma endregion

#pragma region Sprint Variables
	TEnumAsByte<SprintStates> currentSprintState{ disabledSprint };
	float sprintStart{ 0.0f };			//Magnitude of the player's inputs when beginning to sprint (controller only)

	float sprintMult{ 1.25f };		//Speed/Max speed multiplier while sprinting
	float sprintMultCurrent{ 1.0f };//Speed/Max speed multiplier at current
	float sprintMax{ 0.6f };		//Sprint acceleration time
	float sprintDecMax{ 0.9f };		//Sprint deceleration max
	float sprintFieldMult{ 0.85f };	//FoV multiplier while sprinting. You may need to make this more subtle.

	float smoothingSprintProgress{ 0.0f };	//Progress through states/towards state changes while crouching
	float sprintAmp{ 0.0f };				//% progress to/from max offset
#pragma endregion

#pragma region Mantle Variables
	TEnumAsByte<MantleStates> currentMantleState{ noneMantle };
	float smoothingMantleProgress{ 0.0f };	//Progress through states/towards state changes while mantling/preparing to mantle
	float mantleThreshold{ 0.25f };			// mantle check threshold, after which mantling will be confirmed
	float mantleMax{ 1.0f };				//Duration of a mantle

	float mantleVerticalOffset{ 215.0f };	//Vertical offset when checking player destination (in cm, from player midpoint)
	float mantleAngle{ 0.0f };				//Current angle between facing and direction of motion
	float maxMantleAngle{ 30.0f };			//Max permissable angle between player's facing and motion direction for mantling to occur
	float yAmp{ 0.0f };						//Current amplitude of the Y displacement (between 0 and 1)
	float mantleHeightMult{ 10.0f };		//Multiplier on yAmp

	UPROPERTY()
		FVector2D mantleVelocity { 0.0f, 0.0f }; //Velocity recorded while entering mantle state
	UPROPERTY()
		FVector mantlePosition { 0.0f, 0.0f, 0.0f }; //Mantle check position, relative to player
	UPROPERTY()
		FVector mantleDirection { 0.0f, 0.0f, 0.0f }; //Player's last movement direction, minus its z component
#pragma endregion

#pragma endregion

#pragma region Aim Variables
	float smoothX{ 0.0f };		//Aim smoothing in the X axis (disabled for mouse inputs)
	float smoothY{ 0.0f };		//Aim smoothing in the Y axis (disabled for mouse inputs)
	float smoothTime{ 0.2f };	//Total aim smoothing time
	
	float currentYMult{ 20.0f };	//Current aim sensitivity on the Y axis
	float currentXMult{ 20.0f };	//Current aim sensitivity on the X axis
	FVector2D currentXYMult{ 0.0f, 0.0f };
	float aimSpeed{ 20.0f };		//Aim sensitivity
#pragma endregion

#pragma region Motion Variables
	FVector forwardMotion{ 0.0f, 0.0f, 0.0f };
	FVector sideMotion{ 0.0f, 0.0f, 0.0f };
	FVector fullMotion{ 0.0f, 0.0f, 0.0f };
	FVector2D clampedMotion{ 0.0f, 0.0f };	//Player velocity clamped within acceptable parameters
	FVector anchorMotion{ 0.0f, 0.0f, 0.0f };	//Used in motion calculations when the "last" motion is not accurate to expected behaviour
#pragma endregion

#pragma region Physical Traits
	float distanceToGround{ 95.0f };	//Distance from the middle of the capsule to the ground
	float mantleRange{ 95.0f };			//Max distance from which a mantle can be performed
	float coverRadius{ 200.0f };		//Automatic cover activation range

	float foVBase{ 0.0f };
	float foVSprintTarget{ 0.0f };
	float currentFoV{ 0.0f };
	float startFoV{ 0.0f };
#pragma endregion

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
