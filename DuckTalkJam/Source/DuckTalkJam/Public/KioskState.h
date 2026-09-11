// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DayEncounterConfig.h"
#include "Components/AudioComponent.h"
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

	virtual void Init() override;
	virtual void Shutdown() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> MenuMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> GameplayMusic;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayMusic(USoundBase* Music);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopMusic();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMusicVolume(float Volume);

	UPROPERTY(BlueprintReadWrite, Category = "Progress")
	int32 Day = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Progress")
	float Coins = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Items")
	TArray<FGameplayTag> Items;

	UPROPERTY(BlueprintReadWrite, Category = "Items")
	bool b_ItemsInitialized = false;

	UPROPERTY(BlueprintReadWrite, Category = "Progress")
	TArray<FGameplayTag> Flags;

#pragma region Flags

	UFUNCTION(BlueprintCallable)
	void AddFlag(FGameplayTag Flag);

	UFUNCTION(BlueprintCallable)
	bool HasFlag(FGameplayTag Flag);

	UFUNCTION(BlueprintCallable)
	void RemoveFlag(FGameplayTag Flag);

	UFUNCTION(BlueprintCallable)
	void AddItem(FGameplayTag ItemTag);

	UFUNCTION(BlueprintCallable)
	bool HasItem(FGameplayTag ItemTag);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(FGameplayTag ItemTag);

#pragma endregion Flags

private:

	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComponent;

};