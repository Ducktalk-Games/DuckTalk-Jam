// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "GameplayTagContainer.h"
#include "F_DialogueChoice.generated.h"

UENUM(BlueprintType)
enum class EChoiceFunction : uint8
{
    None,
    EndDialogue,
    HangUpPhone,
	Process,
    TurnAway
};

UENUM(BlueprintType)
enum class EChoiceColor : uint8
{
    None,
    Accept,
    Deny
};

USTRUCT(BlueprintType)
struct DUCKTALKJAM_API FF_DialogueChoice
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText ChoiceText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FGameplayTag RequiredFlag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FGameplayTag Flag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName NextRow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	EChoiceFunction ChoiceFunction = EChoiceFunction::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    EChoiceColor ChoiceColor = EChoiceColor::None;
   
    bool IsEmpty() const
    {
        return ChoiceText.IsEmpty()
            && !RequiredFlag.IsValid()
            && !Flag.IsValid()
            && NextRow.IsNone()
            && ChoiceFunction == EChoiceFunction::None;
    }
};