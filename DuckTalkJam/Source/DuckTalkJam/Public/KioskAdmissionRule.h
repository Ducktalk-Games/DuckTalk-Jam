// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

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