// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Test.generated.h"

UCLASS()
class ALLEYESONME_API ATest : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATest();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	AActor* PlayerActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool ObserveTarget();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
