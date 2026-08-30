// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"
#include "F_DialogueChoice.generated.h"

USTRUCT(BlueprintType)
struct DUCKTALKJAM_API FF_DialogueChoice
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText ChoiceText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName Flag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName NextRow;
   
    bool IsEmpty() const
    {
        return ChoiceText.IsEmpty()
            && Flag.IsNone()
            && NextRow.IsNone();
    }
};