// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "Observer.generated.h"

UCLASS()
class ALLEYESONME_API AObserver : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObserver();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
		AActor* PlayerActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
		int searchRange{ 1000 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
		float searchDuration { 2.0f };

	UFUNCTION(BlueprintImplementableEvent, Category = "Detection")
		void onStartDetection();

	UFUNCTION(BlueprintImplementableEvent, Category = "Detection")
		void onEndDetection();

	UFUNCTION(BlueprintImplementableEvent, Category = "Detection")
		void onDetectionComplete();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool ObserveTarget();

	bool seesTarget;
	float detectionStartTime;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
