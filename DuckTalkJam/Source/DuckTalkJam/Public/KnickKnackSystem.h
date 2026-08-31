// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KnickKnackSystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUCKTALKJAM_API UKnickKnackSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKnickKnackSystem();

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Knick Knacks",
		meta = (ToolTip = "Knick knacks owned by the player.")
	)
	TArray<TObjectPtr<AActor>> KnickKnacks;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
