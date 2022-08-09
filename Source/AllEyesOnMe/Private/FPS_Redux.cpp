// Fill out your copyright notice in the Description page of Project Settings.


#include "FPS_Redux.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPS_Redux::AFPS_Redux()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

#pragma region Variable Modification
void AFPS_Redux::OverrideGroundVariables(const float baseSpeedOverride, const float accTimeOverride, const float accLongTimeOverrive, const float decTimeOverride, const float graceTimeOverride)
{
	topSpeed = baseSpeedOverride;
	groundMax = accTimeOverride;
	groundLongMax = accLongTimeOverrive;
	stopMax = decTimeOverride;
	graceMax = graceTimeOverride;
}

void AFPS_Redux::OverrideAerialVariables(const float decTimeOverride, const float terminalInputMultOverride, const float initialInputMultOverride)
{
	airMax = decTimeOverride;
	decayMax = terminalInputMultOverride;
	airMult = initialInputMultOverride;
}

void AFPS_Redux::OverrideCoverVariables(const float heightDiffOverride, const float crouchTimeOverride, const float uncrouchTimeOverride, const float blendTimeOverride, const float crouchMultOverride)
{
	heightMin = heightDiffOverride;
	crouchMax = crouchTimeOverride;
	uncrouchMax = uncrouchTimeOverride;
	blendMax = blendTimeOverride;
	crouchMult = crouchMultOverride;
}

void AFPS_Redux::OverrideAimVariables(const float aimSpeedOverride)
{
	aimSpeed = aimSpeedOverride;
	currentXYMult.Y = aimSpeed;
	currentXYMult.X = aimSpeed;
}

void AFPS_Redux::OverrideSprintVariables(const float sprintMultOverride, const float sprintTimeOverride, const float sprintDecTimeOverride, const float fovChangeOverride)
{
	sprintMult = sprintMultOverride;
	sprintMax = sprintTimeOverride;
	sprintDecMax = sprintDecTimeOverride;
	sprintFieldMult = fovChangeOverride;
}

void AFPS_Redux::OverrideDetectionVariables(const float initDetectRateOverride)
{
	crouchDetectionMult = initDetectRateOverride;
}

void AFPS_Redux::OverridePhysicalTraits(const float groundDistanceOverride, const float mantleRangeOverride, const float coverRadiusOverride)
{
	distanceToGround = groundDistanceOverride;
	mantleRange = mantleRangeOverride;
	coverRadius = coverRadiusOverride;
}

void AFPS_Redux::CrouchOriginPosition(const float setPlayerHeadHeight)
{
	playerHeadPos = setPlayerHeadHeight;
}
void AFPS_Redux::FovOriginAngle(const float cameraFov)
{
	foVBase = cameraFov;
	foVSprintTarget = cameraFov * sprintFieldMult;
	currentFoV = cameraFov;
}
#pragma endregion

#pragma region Inputs
void AFPS_Redux::GroundDetection(const bool airborneCheck)
{
	//Note that the raycast itself is checking if the player is on the ground
	isAirborne = !airborneCheck;
	if (isAirborne)
	{
		currentMotionState = airborne;
	}
	else if (currentMotionState != mantle)
	{
		currentMotionState = grounded;
	}
}

void AFPS_Redux::TimeDifference(const float tempDelta)
{
	deltaTime = tempDelta;
}

void AFPS_Redux::InputType(const bool typeController)
{
	inputTypeController = typeController;
	if (!inputTypeController)
	{
		currentXYMult.Y = aimSpeed;
		smoothY = smoothTime;
		currentXYMult.X = aimSpeed;
		smoothX = smoothTime;
	}
}

void AFPS_Redux::MantleDisplacement(const float progression)
{
	yAmp = progression;
}

void AFPS_Redux::CrouchDisplacement(const float position)
{
	offsetAmp = position;
}

bool AFPS_Redux::IsSprinting() const
{
	{
		if (currentSprintState != constantSprint && currentSprintState != disabledSprint)
		{
			return (true);
		}
		else
		{
			return (false);
		}
	}
}

void AFPS_Redux::SprintDisplacement(const float position)
{
	sprintAmp = position;
}


void AFPS_Redux::DisableMantle()
{
	currentMotionState = airborne;
	currentMantleState = noneMantle;
}
#pragma endregion

#pragma region Camera and Directional Inputs
//Inputs overwritten at the start of every tick
void AFPS_Redux::ControlInputs(const FVector2D motionInput, const FVector2D aimInput)
{
	motion = motionInput;
	if (motionInput.Length() >= 1.0f)
	{
		motion.Normalize();
	}
	aim = aimInput;
	OnTickAimSmoothing();
}

//Direction overwritten after calculating player facing
void AFPS_Redux::PerspectiveInput(const FVector tempForward, const FVector tempRight)
{
	cameraForward = tempForward;
	cameraRight = tempRight;
}
#pragma endregion

#pragma region Outputs
//Outputs player head z position
float AFPS_Redux::CrouchOutput() const
{
	return (playerHeadPosCurrent);
}


FVector2D AFPS_Redux::MotionOutput() const
{
	if (currentGroundState == decelerateGround)
	{
		return(groundVelocityHalt);
	}
	else if (currentMotionState == airborne)
	{
		return (motion);
	}
	else
	{
		return (motion);
	}
}

bool AFPS_Redux::AirborneState() const
{
	if (currentMotionState == airborne)
	{
		return true;
	}
	else
	{
		return false;
	}
}
#pragma endregion

#pragma region Crouch Inputs
//Crouch inputs, called by script when the crouch button is pressed
void AFPS_Redux::CrouchInput()
{
	#pragma region Crouch
	//If the player wasn't in a crouch, or was uncrouching
	if (!isAirborne && (currentCoverState == FullHeight || currentCoverState == RisingCover || currentCoverState == RisingCrouch))
	{
		if (currentCoverState != FullHeight)
		{
			lastOffsetAmp = offsetAmp;
			smoothingCrouchProgress = crouchMax * (1 - (smoothingCrouchProgress / uncrouchMax));
			coverName = "Reset to crouch from rising";
		}
		else
		{
			lastOffsetAmp = 1.0f;
			smoothingCrouchProgress = 0.0f;
			coverName = "Crouching from full height";
		}
		lastCoverState = currentCoverState;
		currentCoverState = DuckingCrouch;
	}
	#pragma endregion

	#pragma region Uncrouch
	//If the player is free to stand up (is ducking and wasn't put there automatically)
	else if (!isAirborne && ((currentCoverState == FullCrouch && lastCoverState != DuckingCover) || currentCoverState == DuckingCrouch))
	{
		if (currentCoverState != FullCrouch)
		{
			lastOffsetAmp = offsetAmp;
			smoothingCrouchProgress = uncrouchMax * (1 - (smoothingCrouchProgress / crouchMax));
			coverName = "Reset to uncrouch from partial crouch";
		}
		else
		{
			lastOffsetAmp = 1.0f;
			smoothingCrouchProgress = 0.0f;
			coverName = "Uncrouching from full crouch";
		}
		lastCoverState = currentCoverState;
		currentCoverState = RisingCrouch;
		//Initialize state change
	}
	else
	{
		coverName = "Not reset, player airborne, mantling or overriden by cover state";
	}
	#pragma endregion

	//The player is otherwise airborne, sprinting or being overridden by The Fear
}

void AFPS_Redux::RecordDirection(const FVector tempDirection)
{
	anchorMotion.X = tempDirection.X;
	anchorMotion.Y = tempDirection.Y;
	anchorMotion.Normalize();
}
#pragma endregion

#pragma region Automated Crouching

#pragma region From Blueprints
float AFPS_Redux::ReturnTopSpeed() const
{
	return (maxSpeedCurrent);
}
void AFPS_Redux::CrouchAdd()
{
	crouchCounter++;
	if (theFear)
	{
		CrouchAutomatic();
	}
}

void AFPS_Redux::CrouchSubtract()
{
	crouchCounter--;
	if (crouchCounter == 0)
	{
		UncrouchAutomatic();
	}
}
#pragma endregion

#pragma region Crouch
//Automated cover system. Overrides a player-initiated crouch.
void AFPS_Redux::CrouchAutomatic()
{
	#pragma region Player not already crouching
	//If the player wasn't in a crouch, or was rising
	if (!isAirborne && (currentCoverState == FullHeight || currentCoverState == RisingCover || currentCoverState == RisingCrouch))
	{
		if (currentCoverState == FullHeight)
		{
			//Crouch smoothing is reset entirely if the player was standing
			lastOffsetAmp = 1.0f;
			smoothingCrouchProgress = 0.0f;
		}
		else
		{
			//If the player was rising, the progress is set to start part way along the curve (the inverse of the completed proportion)
			lastOffsetAmp = offsetAmp;
			smoothingCrouchProgress = crouchMax * (1 - (smoothingCrouchProgress / uncrouchMax));
		}
		lastCoverState = currentCoverState;
		currentCoverState = DuckingCover;
	}
	#pragma endregion
	#pragma region Overriding other states
	//If the player was ducking via intentional crouch
	else if (!isAirborne && currentCoverState == DuckingCrouch)
	{
		//Overrides the state and resets the smoothing crouch progress to the correct relative proportion
		currentCoverState = DuckingCover;
		lastCoverState = DuckingCrouch;
	}
	//If the player was already crouched
	else if (!isAirborne && currentCoverState == FullCrouch)
	{
		//This override prevents the player from uncrouching manually
		//Does not cause any afforded change (maybe it should?)
		lastCoverState = DuckingCover;
	}
	#pragma endregion
}
#pragma endregion

#pragma region Uncrouch
//Uncrouches player from an automated crouch
void AFPS_Redux::UncrouchAutomatic()
{
	//Player must be on the ground, crouching in some form and not have recently manually crouched to get there
	if (!isAirborne && (currentCoverState == FullCrouch || currentCoverState == DuckingCover) && lastCoverState != DuckingCrouch)
	{
		if (currentCoverState != FullCrouch)
		{
			lastOffsetAmp = offsetAmp;
			offsetAmp = 0.0f;
			smoothingCrouchProgress = uncrouchMax * (1 - (smoothingCrouchProgress / crouchMax));
			coverName = "Reset to uncrouch from partial crouch";
		}
		else
		{
			lastOffsetAmp = 1.0f;
			offsetAmp = 0.0f;
			smoothingCrouchProgress = 0.0f;
			coverName = "Uncrouching from full crouch";
		}
		lastCoverState = currentCoverState;
		currentCoverState = RisingCover;
	}
}
#pragma endregion

#pragma region Forced Uncrouch
void AFPS_Redux::UncrouchForced()
{
	if (currentCoverState != FullHeight && currentCoverState != RisingSprint)
	{
		if (currentCoverState == DuckingCrouch || currentCoverState == DuckingCover)
		{
			lastOffsetAmp = offsetAmp;
			offsetAmp = 0.0f;
			smoothingCrouchProgress = uncrouchMax * (1 - (smoothingCrouchProgress / crouchMax));
			coverName = "Reset to uncrouch from partial crouch by sprinting";
		}
		else
		{
			lastOffsetAmp = 1.0f;
			offsetAmp = 0.0f;
			smoothingCrouchProgress = 0.0f;
			coverName = "Uncrouching from full crouch";
		}
		lastCoverState = currentCoverState;
		currentCoverState = RisingSprint;
	}
}
#pragma endregion

#pragma endregion

#pragma region Sprint Controls
void AFPS_Redux::SprintActivation()
{
	if(currentMotionState == grounded && currentSprintState == disabledSprint || currentSprintState == decelerateSprint)
	{
		sprintStart = sprintMultCurrent;
		smoothingSprintProgress = 0.0f;
		currentSprintState = accelerateSprint;
		UncrouchForced();
	}
	startFoV = currentFoV;
}
void AFPS_Redux::SprintDectivation()
{
	if (currentSprintState == constantSprint || currentSprintState == accelerateSprint)
	{
		sprintStart = sprintMultCurrent;
		smoothingSprintProgress = 0.0f;
		currentSprintState = decelerateSprint;
	}
	startFoV = currentFoV;
}
#pragma endregion

#pragma region Debug

FString AFPS_Redux::CurrentCrouchState() const
{
	return coverName;
}

FString AFPS_Redux::CurrentForcedCover() const
{
	if (crouchCounter == 1)
	{
		return ("Forced to cover");
	}
	else if (crouchCounter > 1)
	{
		return ("Entered influence of additional cover object");
	}
	else
	{
		return ("Error: Entered cover region, but cover counter < 1");
	}
}

FString AFPS_Redux::CurrentForcedCoverExit() const
{
	if (crouchCounter > 0)
	{
		return ("Left a cover object's sphere of influence");
	}
	else
	{
		return ("left all cover objects' spheres of influence, resetting to full height");
	}
}

bool AFPS_Redux::IsDecelerating() const
{
	if (currentSprintState == decelerateSprint)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

FVector2D AFPS_Redux::currentAim() const
{
	return (aim);
}

float AFPS_Redux::CrouchTimer() const
{
	return (smoothingCrouchProgress);
}

float AFPS_Redux::OutputChecker() const
{
	return (offsetAmp);
}
#pragma endregion

#pragma region On-Tick behaviour
//The meaty bit

#pragma region MantleCheck
void AFPS_Redux::OnTickMantle(const FVector playerFacing)
{
	if (currentMantleState != mantlingMantle)
	{
		currentMantleState = checkingMantle;
		smoothingMantleProgress += deltaTime;
		if (smoothingMantleProgress >= mantleThreshold)
		{
			smoothingMantleProgress = 0.0f;
			currentMantleState = mantlingMantle;
			currentMotionState = mantle;
			UncrouchForced();
		}
	}
}
#pragma endregion

#pragma region Reset Mantling
void AFPS_Redux::ResetMantle()
{
	currentMantleState = noneMantle;
	smoothingMantleProgress = 0.0f;
}
#pragma endregion

#pragma region Player Aim Smoothing
void AFPS_Redux::OnTickAimSmoothing()
{
	#pragma region Aim Acceleration Priming
	//Prepares aim for smooth acceleration (if on controller)
	if (inputTypeController && aim.X == 0.0f)
	{
		smoothX = 0.0f;
	}
	if (inputTypeController && aim.Y == 0.0f)
	{
		smoothY = 0.0f;
	}
	#pragma endregion

	#pragma region Camera Rotation
	//Vertical aim calculation and smoothing
	if (smoothY < smoothTime && inputTypeController)
	{
		currentXYMult.Y = FMath::Lerp(0.0f, aimSpeed, smoothY / smoothTime);
		smoothY += deltaTime;
		if (smoothY >= smoothTime)
		{
			currentXYMult.Y = aimSpeed;
		}
	}

	//Horizontal aim calculation and smoothing
	if (smoothX < smoothTime && inputTypeController)
	{
		currentXYMult.X = FMath::Lerp(0.0f, aimSpeed, smoothX / smoothTime);
		smoothX += deltaTime;
		if (smoothY >= smoothTime)
		{
			currentXYMult.X = aimSpeed;
		}
	}
	#pragma endregion
}
#pragma endregion

#pragma region Player Motion
void AFPS_Redux::OnTickMotion()
{
	#pragma region Movement State Considerations (The Smoothening)
	//Works through the different motion considerations
	//and applies smoothing relevant to their current states

	Acceleration();	//First, have controls been released, or has the player been idle?

	Grounded();	//Then, calculate speedMult, based on current acceleration

	SprintingSpeed(); //Then the sprint speed multiplier

	CrouchedSpeed();

	switch (currentMotionState)	//Then, determine which state the player is in
	{
	case grounded:
		break;
	case airborne:
		Airborne();
		break;
	case mantle:
		break;
	}
	#pragma endregion

	//Finally, use the top speed, acceleration, sprint multiplier and crouch multiplier to
	//set the effective top speed
	maxSpeedCurrent = topSpeed * speedMult * sprintMultCurrent * crouchMultCurrent;
}
#pragma endregion

#pragma region Stopping and Starting (and figuring out which is which)
void AFPS_Redux::Acceleration()
{
	//If the below is true, the player is not inputting any directional control
	if (motion.Length() == 0.0f)
	{
		//Checks whether the player is in the "ready to speed up" state and has fully come to a halt
		//Resets the player's acceleration every frame these conditions are met
		//Velocity hasn't been set this frame yet, so we can use it to consider last frame's velocity
		//regardless of grounded/airborne considerations
		if (currentGroundState == accelerateGround && velocity.Length() == 0.0f)
		{
			smoothingGroundProgress = 0.0f;
		}
		//Otherwise. they haven't come to a halt yet.
		//They should either start that process or already be in it
		else if (currentGroundState != decelerateGround)
		{
			currentGroundState == decelerateGround;
		}
	}
	//If the player is in motion, the grace period for "no input" is continually reset
	else
	{
		grace = 0.0f;
		if (currentGroundState == decelerateGround)
		{
			//In this case, the player was decelerating (or in the grace period before deceleration)
			//and input a directional control before coming to a halt.
			//This has a very small window and is largely to prevent sudden halts when crossing the axes on a controller.
			currentGroundState = constantGround;
			smoothingGroundProgress = 0.0f;
		}
	}

	//If the player has stopped, primes them to accelerate back to top speed
	//They must not be moving and be on the ground.
	//There is a brief transitional period in which this can be cancelled by the above code
	if (velocity.Length() == 0.0f)
	{
		grace += deltaTime;
		if (grace > graceMax)
		{
			currentGroundState = accelerateGround;
		}
	}
}
#pragma endregion

#pragma region Motion Smoothing

#pragma region Grounded Motion Smoothing
void AFPS_Redux::Grounded()
{
	switch (currentGroundState)
	{
	#pragma region Constant Speed
	case constantGround:
		//Movement at top speed
		speedMult = 1.0f;
		smoothingGroundProgress = 0.0f;

		groundVelocityHalt.X = motion.X;
		groundVelocityHalt.Y = motion.Y;
		break;
	#pragma endregion
	#pragma region Accelerating From Halted
	case accelerateGround:
		//Movement when accelerating from a fully stopped state
		speedMult = FMath::Lerp(0.0f, 1.0f, smoothingGroundProgress / groundMax);
		smoothingGroundProgress += deltaTime;
		if (smoothingGroundProgress > groundMax)
		{
			currentGroundState = constantGround;
			smoothingGroundProgress = 0.0f;
		}

		groundVelocityHalt.X = motion.X;
		groundVelocityHalt.Y = motion.Y;
		break;
	#pragma endregion
	#pragma region Accelerating From Long Fall
	case accelerateLongGround:
		//Movement when accelerating from a long fall
		speedMult = FMath::Lerp(0.0f, 1.0f, smoothingGroundProgress / groundLongMax);
		smoothingGroundProgress += deltaTime;
		if (smoothingGroundProgress > groundLongMax)
		{
			speedMult = 1.0f;
			currentGroundState = constantGround;
			smoothingGroundProgress = 0.0f;
		}

		groundVelocityHalt.X = motion.X;
		groundVelocityHalt.Y = motion.Y;
		break;
	#pragma endregion
	#pragma region Decelerating
	case decelerateGround:
		//Deceleration (does not record groundVelocityHalt, it's already halting)
		speedMult = FMath::Lerp(1.0f, 0.0f, smoothingGroundProgress / stopMax);
		smoothingGroundProgress += deltaTime;
		if (smoothingGroundProgress >= stopMax)
		{
			speedMult = 0.0f;
			currentGroundState = accelerateGround;
			smoothingGroundProgress = 0.0f;
		}
		break;
	#pragma endregion
	}
	GroundVelocityLast.X = motion.X;
	GroundVelocityLast.Y = motion.Y;
	smoothingAirProgress = 0.0f;
}
#pragma endregion

#pragma region Airborne Motion Smoothing
void AFPS_Redux::Airborne()
{
	switch (currentAirState)
	{
	#pragma region Constant Speed
	case constantAir:
		//aerial coasting
		//The below should largely be true, but if not, the player wouldn't experience the effect anyway - no need to set it up.
		if (motion.Length() > 0.0f)
		{
			currentGroundState = accelerateLongGround;
		}
		fullMotion.X = (airMult * forwardMotion.X) + (airMult * sideMotion.X);
		fullMotion.Y = (airMult * forwardMotion.Y) + (airMult * sideMotion.Y);
		break;
	#pragma endregion
	#pragma region Decaying Speed
	case decelerateAir:
		motionMagnitudeVector.X = FMath::Lerp(GroundVelocityLast.X, decayMax, smoothingAirProgress / airMax);
		fullMotion.X = motionMagnitudeVector.X + (airMult * forwardMotion.X) + (airMult * sideMotion.X);
		motionMagnitudeVector.Y = FMath::Lerp(GroundVelocityLast.Y, decayMax, smoothingAirProgress / airMax);
		fullMotion.Y = motionMagnitudeVector.Y + (airMult * forwardMotion.Y) + (airMult * sideMotion.Y);
		smoothingAirProgress += deltaTime;
		if (smoothingAirProgress >= airMax)
		{
			currentAirState = constantAir;
		}
		break;
	#pragma endregion
	}
}
#pragma endregion

#pragma endregion

#pragma region Top Speed Calculations

#pragma region Crouched Speed
void AFPS_Redux::CrouchedSpeed()
{
	switch (currentCoverState)
	{
	case FullCrouch:
		crouchMultCurrent = crouchMult;
		break;
	case DuckingCover:
		smoothingCrouchProgress += deltaTime;
		if (smoothingCrouchProgress >= crouchMax)
		{
			smoothingCrouchProgress = 0.0f;
			currentCoverState = FullCrouch;
			lastCoverState = DuckingCover;
			crouchMultCurrent = crouchMult;
			playerHeadPosCurrent = playerHeadPos + heightMin;
			offsetAmp = 0.0f;
		}
		else
		{
			crouchMultCurrent = FMath::Lerp(1.0f, crouchMult, smoothingCrouchProgress / crouchMax);
			playerHeadPosCurrent = FMath::Lerp(playerHeadPos + (heightMin * ( 1 - lastOffsetAmp)), playerHeadPos + heightMin, offsetAmp);
		}
		break;
	case DuckingCrouch:
		smoothingCrouchProgress += deltaTime;
		if (smoothingCrouchProgress >= crouchMax)
		{
			smoothingCrouchProgress = 0.0f;
			currentCoverState = FullCrouch;
			lastCoverState = DuckingCrouch;
			crouchMultCurrent = crouchMult;
			playerHeadPosCurrent = playerHeadPos + heightMin;
			offsetAmp = 0.0f;
		}
		else
		{
			crouchMultCurrent = FMath::Lerp(1.0f, crouchMult, smoothingCrouchProgress / crouchMax);
			playerHeadPosCurrent = FMath::Lerp(playerHeadPos + (heightMin * ( 1 - lastOffsetAmp)), playerHeadPos + heightMin, offsetAmp);
		}
		break;
	case RisingCover:
		smoothingCrouchProgress += deltaTime;
		if (smoothingCrouchProgress >= uncrouchMax)
		{
			smoothingCrouchProgress = 0.0f;
			currentCoverState = FullHeight;
			lastCoverState = RisingCover;
			crouchMultCurrent = 1.0f;
			playerHeadPosCurrent = playerHeadPos;
			offsetAmp = 0.0f;
		}
		else
		{
			crouchMultCurrent = FMath::Lerp(crouchMult, 1.0f, smoothingCrouchProgress / uncrouchMax);
			playerHeadPosCurrent = FMath::Lerp(playerHeadPos + (heightMin * (lastOffsetAmp)), playerHeadPos, offsetAmp);
		}
		break;
	case RisingCrouch:
		smoothingCrouchProgress += deltaTime;
		if (smoothingCrouchProgress >= uncrouchMax)
		{
			smoothingCrouchProgress = 0.0f;
			currentCoverState = FullHeight;
			lastCoverState = RisingCrouch;
			crouchMultCurrent = 1.0f;
			playerHeadPosCurrent = playerHeadPos;
			offsetAmp = 0.0f;
		}
		else
		{
			crouchMultCurrent = FMath::Lerp(crouchMult, 1.0f, smoothingCrouchProgress / uncrouchMax);
			playerHeadPosCurrent = FMath::Lerp(playerHeadPos + (heightMin * (lastOffsetAmp)), playerHeadPos, offsetAmp);
		}
		break;
	case RisingSprint:
		smoothingCrouchProgress += deltaTime;
		if (smoothingCrouchProgress >= uncrouchMax)
		{
			smoothingCrouchProgress = 0.0f;
			currentCoverState = FullHeight;
			lastCoverState = RisingSprint;
			crouchMultCurrent = 1.0f;
			playerHeadPosCurrent = playerHeadPos;
			offsetAmp = 0.0f;
		}
		else
		{
			crouchMultCurrent = FMath::Lerp(crouchMult, 1.0f, smoothingCrouchProgress / uncrouchMax);
			playerHeadPosCurrent = FMath::Lerp(playerHeadPos + (heightMin * (lastOffsetAmp)), playerHeadPos, smoothingCrouchProgress / uncrouchMax);
		}
		break;
	}
}
#pragma endregion

#pragma region Sprinting Speed
void AFPS_Redux::SprintingSpeed()
{
	switch (currentSprintState) 
	{
	case constantSprint:
		sprintMultCurrent = sprintMult;
		break;
	case accelerateSprint:
		smoothingSprintProgress += deltaTime;
		if (smoothingSprintProgress >= sprintMax)
		{
			smoothingSprintProgress = 0.0f;
			currentSprintState = constantSprint;
			sprintMultCurrent = sprintMult;
			currentFoV = foVSprintTarget;
		}
		else
		{
			sprintMultCurrent = FMath::Lerp(sprintStart, sprintMult, sprintAmp);
			currentFoV = FMath::Lerp(startFoV, foVSprintTarget, sprintAmp);
		}
		break;
	case decelerateSprint:
		smoothingSprintProgress += deltaTime;
		if (smoothingSprintProgress >= sprintDecMax)
		{
			smoothingSprintProgress = 0.0f;
			currentSprintState = disabledSprint;
			sprintMultCurrent = 1.0f;
			currentFoV = foVBase;
		}
		else
		{
			sprintMultCurrent = FMath::Lerp(sprintStart, 1.0f, sprintAmp);
			currentFoV = FMath::Lerp(startFoV, foVBase, sprintAmp);
		}
		break;
	case disabledSprint:
		sprintMultCurrent = 1.0f;
		break;
	}
}
#pragma endregion

#pragma endregion

#pragma endregion


// Called when the game starts or when spawned
void AFPS_Redux::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPS_Redux::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPS_Redux::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

