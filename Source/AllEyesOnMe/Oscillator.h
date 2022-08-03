// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Math/Quat.h"
#include "Math/UnrealMathUtility.h"
#include "Oscillator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALLEYESONME_API UOscillator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOscillator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	FVector startPosition;
	FVector trueNorthPosition;
	FVector trueSouthPosition;

	FRotator startRotation;
	FRotator trueNorthRotation;
	FRotator trueSouthRotation;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FVector northPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
		FRotator northRotation;
};
