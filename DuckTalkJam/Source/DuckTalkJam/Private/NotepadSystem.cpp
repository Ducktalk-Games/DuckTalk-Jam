// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#include "NotepadSystem.h"

UNotepadSystem::UNotepadSystem()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNotepadSystem::BeginPlay()
{
	Super::BeginPlay();
}

void UNotepadSystem::SetNotepadText(const FString& NewText)
{
	NotepadText = NewText;
}

void UNotepadSystem::ClearNotepad()
{
	NotepadText.Empty();
}

TArray<FString> UNotepadSystem::GetLines() const
{
	TArray<FString> Lines;

	// false = preserve empty lines.
	NotepadText.ParseIntoArrayLines(Lines, false);

	return Lines;
}