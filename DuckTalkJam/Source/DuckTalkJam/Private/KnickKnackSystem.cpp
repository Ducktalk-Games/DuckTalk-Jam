// Fill out your copyright notice in the Description page of Project Settings.


#include "KnickKnackSystem.h"

// Sets default values for this component's properties
UKnickKnackSystem::UKnickKnackSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UKnickKnackSystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UKnickKnackSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}