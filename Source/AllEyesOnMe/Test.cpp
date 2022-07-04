// Fill out your copyright notice in the Description page of Project Settings.


#include "Test.h"

// Sets default values
ATest::ATest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATest::BeginPlay()
{
	Super::BeginPlay();
}

bool ATest::ObserveTarget()
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
	targetVector.Normalize();

	float dotProduct = FVector::DotProduct(GetActorForwardVector(), targetVector);

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *playerPos.ToString());
	UE_LOG(LogTemp, Warning, TEXT("%f"), dotProduct);

	return true;
}

// Called every frame
void ATest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ObserveTarget())
	{
		//UE_LOG(LogTemp, Warning, TEXT("True"));
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("False"));
	}

}

