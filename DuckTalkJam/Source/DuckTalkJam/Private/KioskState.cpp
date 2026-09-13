// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#include "KioskState.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void UKioskState::Init()
{
	Super::Init();

	MusicVolume = 1.0f;
}

void UKioskState::InitializeMusicComponents()
{
	if (MusicComponentA && MusicComponentB) return;

	USoundBase* InitialSound = MenuMusic ? MenuMusic : GameplayMusic;
	if (!InitialSound) return;

	MusicComponentA = UGameplayStatics::CreateSound2D(
		this,
		InitialSound,
		MusicVolume,
		1.0f,
		0.0f,
		nullptr,
		true,   // Persist across level transition
		false   // Don't auto destroy
	);

	MusicComponentB = UGameplayStatics::CreateSound2D(
		this,
		InitialSound,
		MusicVolume,
		1.0f,
		0.0f,
		nullptr,
		true,
		false
	);

	if (MusicComponentA) MusicComponentA->Stop();
	if (MusicComponentB) MusicComponentB->Stop();

	ActiveMusicComponent = MusicComponentA;
	InactiveMusicComponent = MusicComponentB;
}

void UKioskState::Shutdown()
{
	if (MusicComponentA)
	{
		MusicComponentA->Stop();
		MusicComponentA = nullptr;
	}

	if (MusicComponentB)
	{
		MusicComponentB->Stop();
		MusicComponentB = nullptr;
	}

	ActiveMusicComponent = nullptr;
	InactiveMusicComponent = nullptr;

	Super::Shutdown();
}

void UKioskState::PlayMusic(USoundBase* Music, float FadeDuration)
{
	if (!Music) return;

	InitializeMusicComponents();

	if (!ActiveMusicComponent || !InactiveMusicComponent) return;

	FadeDuration = FMath::Max(0.0f, FadeDuration);

	if (ActiveMusicComponent->IsPlaying() && ActiveMusicComponent->GetSound() == Music) return;

	InactiveMusicComponent->Stop();
	InactiveMusicComponent->SetSound(Music);

	if (ActiveMusicComponent->IsPlaying()) ActiveMusicComponent->FadeOut(FadeDuration, 0.0f, EAudioFaderCurve::Linear);
	InactiveMusicComponent->FadeIn(FadeDuration, MusicVolume, 0.0f, EAudioFaderCurve::Linear);

	Swap(ActiveMusicComponent, InactiveMusicComponent);
}

void UKioskState::StopMusic(float FadeDuration)
{
	if (!ActiveMusicComponent) return;

	FadeDuration = FMath::Max(0.0f, FadeDuration);
	if (ActiveMusicComponent->IsPlaying())
	{
		if (FadeDuration > 0.0f) ActiveMusicComponent->FadeOut(FadeDuration, 0.0f, EAudioFaderCurve::Linear);
		else ActiveMusicComponent->Stop();
	}
}

void UKioskState::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);

	if (MusicComponentA) MusicComponentA->SetVolumeMultiplier(MusicVolume);
	if (MusicComponentB) MusicComponentB->SetVolumeMultiplier(MusicVolume);
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
	Flags.Remove(Flag);
}

void UKioskState::AddItem(FGameplayTag ItemTag)
{
	if (!HasItem(ItemTag)) Items.Add(ItemTag);
}

bool UKioskState::HasItem(FGameplayTag ItemTag)
{
	return Items.Contains(ItemTag);
}

void UKioskState::RemoveItem(FGameplayTag ItemTag)
{
	Items.Remove(ItemTag);
}