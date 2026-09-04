// Copyright (c) 2026 Borna Hukman. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Kiosk Character")
	void StartEncounter();

	virtual void StartEncounter_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Kiosk Character")
	void GrantedEntry();

	virtual void GrantedEntry_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Kiosk Character")
	void RejectedEntry();

	virtual void RejectedEntry_Implementation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};