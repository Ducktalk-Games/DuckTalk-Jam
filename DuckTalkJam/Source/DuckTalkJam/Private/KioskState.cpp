// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "KioskState.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DayEncounterConfig.h"

void UKioskState::Init()
{
	Super::Init();
}

void UKioskState::Shutdown()
{
	if (MusicComponent)
	{
		MusicComponent->Stop();
	}

	Super::Shutdown();
}

void UKioskState::PlayMusic(USoundBase* Music)
{
    if (!Music) return;

    if (MusicComponent &&
        MusicComponent->IsPlaying() &&
        MusicComponent->GetSound() == Music) return;

    if (MusicComponent)
    {
        MusicComponent->Stop();
        MusicComponent = nullptr;
    }

    MusicComponent = UGameplayStatics::SpawnSound2D(
        this,
        Music,
        1.0f,   // Volume
        1.0f,   // Pitch
        0.0f,   // Start time
        nullptr,
        true,   // Persist Across Level Transition
        false   // Auto Destroy
    );
}

void UKioskState::StopMusic()
{
    if (MusicComponent)
    {
        MusicComponent->Stop();
        MusicComponent = nullptr;
    }
}

void UKioskState::SetMusicVolume(float Volume)
{
    if (MusicComponent)
    {
        MusicComponent->SetVolumeMultiplier(
            FMath::Clamp(Volume, 0.0f, 1.0f)
        );
    }
}

void UKioskState::AddFlag(FGameplayTag Flag)
{
	if (!Flags.Contains(Flag)) Flags.Add(Flag);
}

bool UKioskState::HasFlag(FGameplayTag Flag)
{
	return Flags.Contains(Flag);
}

void UKioskState::RemoveFlag(FGameplayTag Flag)
{
	if (Flags.Contains(Flag)) Flags.Remove(Flag);
}

void UKioskState::AddItem(FGameplayTag ItemTag)
{
	if (!HasItem(ItemTag)) Items.Add(ItemTag);
}

bool UKioskState::HasItem(FGameplayTag ItemTag)
{
	if (Items.Contains(ItemTag)) return true;
	return false;
}

void UKioskState::RemoveItem(FGameplayTag ItemTag)
{
	if (HasItem(ItemTag)) Items.Remove(ItemTag);
}