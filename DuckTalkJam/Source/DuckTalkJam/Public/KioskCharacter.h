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

#pragma region Clothing

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<USceneComponent> HatRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<UStaticMeshComponent> HatMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clothing|Sockets")
	FName HatSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<USceneComponent> GlassesRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<UStaticMeshComponent> GlassesMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clothing|Sockets")
	FName GlassesSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<USceneComponent> BeardRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<UStaticMeshComponent> BeardMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clothing|Sockets")
	FName BeardSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<USceneComponent> PatchRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<UStaticMeshComponent> PatchMesh;
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Clothing|Sockets"		
	)
	FName PatchSocket;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<USceneComponent> TieRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Clothing|Components")
	TObjectPtr<UStaticMeshComponent> TieMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Clothing|Sockets")
	FName TieSocket;
#pragma endregion

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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Kiosk Character")
	void StartEncounter();

	virtual void StartEncounter_Implementation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};