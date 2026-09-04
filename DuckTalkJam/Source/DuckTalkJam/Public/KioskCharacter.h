// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "PlayerBase.h"
#include "KioskCharacter.generated.h"

UCLASS()
class DUCKTALKJAM_API AKioskCharacter : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKioskCharacter();

	UPROPERTY(BlueprintReadOnly, Category = "Player Reference")
	TObjectPtr<APlayerBase> PlayerReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	FGameplayTagContainer Traits;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Character",
		meta = (ToolTip = "The Data Table containing all dialogue rows for this character.")
	)
	TObjectPtr<UDataTable> ConversationTable;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};