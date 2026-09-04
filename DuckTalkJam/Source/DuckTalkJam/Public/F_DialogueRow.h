// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "F_DialogueChoice.h"
#include "F_DialogueRow.generated.h"

USTRUCT(BlueprintType)
struct DUCKTALKJAM_API FF_DialogueRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText Speaker;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText DialogueText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FText SetsFlag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FName NextRow;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    bool b_AutoplayNode = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FF_DialogueChoice> Choices;

    FF_DialogueRow() = default;
};