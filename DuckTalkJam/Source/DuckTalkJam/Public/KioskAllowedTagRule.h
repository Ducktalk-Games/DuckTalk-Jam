// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "KioskRule.h"
#include "GameplayTagContainer.h"
#include "KioskAllowedTagRule.generated.h"

class AKioskCharacter;

UCLASS(Blueprintable)
class DUCKTALKJAM_API UKioskAllowedTagRule : public UKioskRule
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AllowedTags;

	virtual bool IsViolatedBy_Implementation(FGameplayTagContainer Traits) const override;
};