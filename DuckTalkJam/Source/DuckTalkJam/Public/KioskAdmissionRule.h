// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KioskRule.h"
#include "KioskAdmissionRule.generated.h"

/**
 * 
 */
UCLASS()
class DUCKTALKJAM_API UKioskAdmissionRule : public UKioskRule
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rule")
	TArray<TSubclassOf<AKioskCharacter>> ExpectedCharacters;

	virtual bool IsViolatedBy_Implementation(AKioskCharacter* Character) const override;
	
};