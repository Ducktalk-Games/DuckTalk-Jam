// Copyright (c) 2026 Borna Hukman. All Rights Reserved.


#include "PlayerItem.h"

// Sets default values
APlayerItem::APlayerItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}