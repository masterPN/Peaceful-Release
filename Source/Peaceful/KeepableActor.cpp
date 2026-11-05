// Fill out your copyright notice in the Description page of Project Settings.


#include "KeepableActor.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AKeepableActor::AKeepableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	RootComponent = BoxComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));
}

// Called when the game starts or when spawned
void AKeepableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKeepableActor::OnPickedUp()
{
	if (!AudioComponent) {
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(this, AudioComponent, GetActorLocation());
}
