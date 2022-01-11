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

// Called every frame
void AKeepableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(IsHidden() && !IsAudioPlayed){
		UGameplayStatics::PlaySoundAtLocation(this, AudioComponent, GetActorLocation());
		IsAudioPlayed = true;
		SetActorTickEnabled(false);
	}
}