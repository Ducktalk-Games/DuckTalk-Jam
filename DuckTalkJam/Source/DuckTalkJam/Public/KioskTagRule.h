// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KioskRule.h"
#include "GameplayTagContainer.h"
#include "KioskTagRule.generated.h"

class AKioskCharacter;

UCLASS(Blueprintable)
class DUCKTALKJAM_API UKioskTagRule : public UKioskRule
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ForbiddenTag;

	virtual bool IsViolatedBy_Implementation(AKioskCharacter* Character) const override;
};