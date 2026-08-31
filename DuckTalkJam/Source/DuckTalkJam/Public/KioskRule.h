#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "KioskCharacter.h"
#include "KioskRule.generated.h"

UCLASS(Abstract, Blueprintable)
class DUCKTALKJAM_API UKioskRule : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AmbiguousDescription;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsViolatedBy(AKioskCharacter* Character) const;

	virtual bool IsViolatedBy_Implementation(AKioskCharacter* Character) const;
};