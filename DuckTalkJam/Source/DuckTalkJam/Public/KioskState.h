// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DayEncounterConfig.h"
#include "Engine/GameInstance.h"
#include "KioskState.generated.h"

/**
 * 
 */
UCLASS()
class DUCKTALKJAM_API UKioskState : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	float Coins = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	TArray<FGameplayTagContainer> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	bool b_ItemsInitialized = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progress")
	TArray<FGameplayTag> Flags;

#pragma region Flags

	UFUNCTION(BlueprintCallable)
	void AddFlag(FGameplayTag Flag);

	UFUNCTION(BlueprintCallable)
	bool HasFlag(FGameplayTag Flag);

	UFUNCTION(BlueprintCallable)
	void RemoveFlag(FGameplayTag Flag);

	UFUNCTION(BlueprintCallable)
	void AddItem(FGameplayTagContainer ItemTag);

	UFUNCTION(BlueprintCallable)
	bool HasItem(FGameplayTagContainer ItemTag);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(FGameplayTagContainer ItemTag);

#pragma endregion Flags

};