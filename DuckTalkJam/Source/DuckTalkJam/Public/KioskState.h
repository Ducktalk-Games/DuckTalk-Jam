// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DayEncounterConfig.h"
#include "Components/AudioComponent.h"
#include "Engine/GameInstance.h"
#include "KioskState.generated.h"

UCLASS()
class DUCKTALKJAM_API UKioskState : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;
	virtual void Shutdown() override;

#pragma region Audio

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> MenuMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> GameplayMusic;

	UPROPERTY(BlueprintReadOnly, Category = "Audio")
	float MusicVolume = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayMusic(USoundBase* Music, float FadeDuration = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopMusic(float FadeDuration = 2.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetMusicVolume(float Volume);

#pragma endregion Audio

#pragma region Progress

	UPROPERTY(BlueprintReadWrite, Category = "Progress")
	int32 Day = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Progress")
	float Coins = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Items")
	TArray<FGameplayTag> Items;

	UPROPERTY(BlueprintReadWrite, Category = "Items")
	bool b_ItemsInitialized = false;

	UPROPERTY(BlueprintReadWrite, Category = "Progress")
	TArray<FGameplayTag> Flags;

#pragma endregion Progress

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

	void InitializeMusicComponents();

	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComponentA;

	UPROPERTY()
	TObjectPtr<UAudioComponent> MusicComponentB;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ActiveMusicComponent;

	UPROPERTY()
	TObjectPtr<UAudioComponent> InactiveMusicComponent;
};