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

FString UNotepadSystem::SetNotepadText(const FString& NewText)
{
	FString LimitedText = NewText;
	TArray<FString> Lines;
	LimitedText.ParseIntoArrayLines(Lines, false);

	if (Lines.Num() > MaxLines)
	{
		Lines.SetNum(MaxLines);
		LimitedText = FString::Join(Lines, TEXT("\n"));
	}

	NotepadText = LimitedText;
	return NotepadText;
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