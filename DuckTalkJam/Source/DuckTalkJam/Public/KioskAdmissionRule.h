// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KioskRule.h"
#include "GameplayTagContainer.h"
#include "KioskAdmissionRule.generated.h"

/**
 * 
 */
UCLASS()
class DUCKTALKJAM_API UKioskAdmissionRule : public UKioskRule
{
	GENERATED_BODY()

public:

	virtual bool IsViolatedBy_Implementation(FGameplayTagContainer Traits) const override;
	
};