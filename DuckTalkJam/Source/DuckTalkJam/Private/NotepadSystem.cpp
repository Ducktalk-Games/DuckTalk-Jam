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
	TArray<FString> InputLines;
	NewText.ParseIntoArrayLines(InputLines, false);

	TArray<FString> WrappedLines;
	for (const FString& Line : InputLines)
	{
		if (Line.IsEmpty())
		{
			WrappedLines.Add(TEXT(""));
			continue;
		}

		int32 StartIndex = 0;
		while (StartIndex < Line.Len())
		{
			if (WrappedLines.Num() >= MaxLines) break;
			const int32 CharactersToTake = FMath::Min(MaxCharactersPerLine, Line.Len() - StartIndex);
			WrappedLines.Add(Line.Mid(StartIndex, CharactersToTake));
			StartIndex += CharactersToTake;
		}

		if (WrappedLines.Num() >= MaxLines) break;
	}

	FString LimitedText = FString::Join(WrappedLines, TEXT("\n"));
	if (LimitedText.Len() > MaxCharacters) LimitedText.LeftInline(MaxCharacters);

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