// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NotepadSystem.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUCKTALKJAM_API UNotepadSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UNotepadSystem();

	UFUNCTION(BlueprintCallable, Category = "Notepad")
	FString SetNotepadText(const FString& NewText);

	UFUNCTION(BlueprintPure, Category = "Notepad")
	const FString& GetNotepadText() const
	{
		return NotepadText;
	}

	UFUNCTION(BlueprintCallable, Category = "Notepad")
	void ClearNotepad();

	UFUNCTION(BlueprintPure, Category = "Notepad")
	TArray<FString> GetLines() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notepad")
	int32 MaxLines = 20;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	FString NotepadText;

};