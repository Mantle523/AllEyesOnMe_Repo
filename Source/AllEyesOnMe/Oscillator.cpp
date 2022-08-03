// Fill out your copyright notice in the Description page of Project Settings.

#include "Oscillator.h"

// Sets default values for this component's properties
UOscillator::UOscillator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;	
}


// Called when the game starts
void UOscillator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AActor* actor = GetOwner();
	startPosition = actor->GetActorLocation();
	trueNorthPosition = startPosition + northPosition;
	trueSouthPosition = startPosition - northPosition;

	startRotation = actor->GetActorRotation();
	trueNorthRotation = startRotation + northRotation;
	trueSouthRotation = startRotation - northRotation;
}


// Called every frame
void UOscillator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* actor = GetOwner();
	FVector newPos;
	FRotator newRot;

	//get the alpha value for the sine function
	float alpha = FMath::Sin(GetWorld()->TimeSeconds);
	if (alpha >= 0)
	{
		newPos = FMath::Lerp(startPosition, trueNorthPosition, alpha);
		newRot = FMath::Lerp(startRotation, trueNorthRotation, alpha);
	}
	else
	{
		newPos = FMath::Lerp(startPosition, trueSouthPosition, -alpha);
		newRot = FMath::Lerp(startRotation, trueSouthRotation, -alpha);
	}

	FQuat QuatRot = FQuat(newRot);
	
	actor->SetActorLocationAndRotation(newPos, QuatRot);
}

