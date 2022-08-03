// Fill out your copyright notice in the Description page of Project Settings.


#include "Observer.h"

// Sets default values
AObserver::AObserver()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	seesTarget = false;

}

// Called when the game starts or when spawned
void AObserver::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AObserver::ObserveTarget()
{
	if (PlayerActor == nullptr)
	{
		PlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();

		return false;
	}
	//Get the players pos relative to this actor
	FVector playerPos = PlayerActor->GetActorLocation();
	FVector actorPos = this->GetActorLocation();

	FVector targetVector = playerPos - actorPos;

	//If the player is too far away, we can skip everything else
	if (targetVector.Size() > searchRange)
	{
		return false;
	}

	targetVector.Normalize();

	float dotProduct = FVector::DotProduct(GetActorForwardVector(), targetVector);

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *playerPos.ToString());
	if (dotProduct <= 0.85f)
	{
		return false;
	}

	//check for obstacles
	FHitResult RV_Hit(ForceInit);
	FCollisionQueryParams RV_TraceParams =
		FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);

	RV_TraceParams.bTraceComplex = true;
	//RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = true;
	RV_TraceParams.AddIgnoredActor(this);

	bool trace = GetWorld()->LineTraceSingleByChannel(RV_Hit, actorPos, playerPos, ECC_Pawn, RV_TraceParams);

	if (!trace)
	{
		return false;
	}

	if (RV_Hit.GetActor() == PlayerActor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("%f"), dotProduct);
		return true;
	}

	return false;
}

// Called every frame
void AObserver::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool seesTargetThisFrame = ObserveTarget();
	if (seesTargetThisFrame == seesTarget) //no change this frame
	{
		if (seesTargetThisFrame) //I continue to see the target
		{
			//if the target has been observed for the full duration, complete the detection
			float currentTime = GetWorld()->GetTimeSeconds();

			if ((detectionStartTime + searchDuration) <= currentTime)
			{
				onDetectionComplete();
				UE_LOG(LogTemp, Warning, TEXT("Detection Complete"));
			}
		}

		return;
	}

	if (seesTargetThisFrame)//we have just this frame started to see the target
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected"));
		onStartDetection();

		detectionStartTime = GetWorld()->GetTimeSeconds();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Detection Ended"));
		onEndDetection();
	}

	seesTarget = seesTargetThisFrame;
}

