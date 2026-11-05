// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/Platform.h"
#include "Math/MathFwd.h"
#include "Peaceful/KeepableActor.h"
#include "Peaceful/OpenDoor.h"
#include "TannyCharacter.h"
#include "UObject/NameTypes.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UInteractionComponent::TriggerInteraction()
{
	ATannyCharacter *Owner = Cast<ATannyCharacter>(GetOwner());
	if (!Owner) {
		return;
	}

	FHitResult HitResult = GetFirstBodyInReach();
	AActor *ActorHit = HitResult.GetActor();

	if (ActorHit) {
		AKeepableActor *Keepable = Cast<AKeepableActor>(ActorHit);
		if (Keepable != nullptr) {
			Owner->AddKey(Keepable->GetKeyId());
			Keepable->OnPickedUp();
			ActorHit->SetActorHiddenInGame(true);
			return;
		}
	}

	UOpenDoor* Door = ActorHit->FindComponentByClass<UOpenDoor>();

	if (Door != nullptr) {
		Door->Interact(Owner);
	}
}


FHitResult UInteractionComponent::GetFirstBodyInReach() const
{
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetReachStart(),
		GetReachEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel2),
		TraceParams
	);

	return Hit;
}


FVector UInteractionComponent::GetReachStart() const
{
	FVector Location;
	FRotator Rotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT Location, OUT Rotation);

	return Location;
}


FVector UInteractionComponent::GetReachEnd() const
{
	FVector Location;
	FRotator Rotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT Location, OUT Rotation);

	return Location+(Rotation.Vector()*ReachRange);
}

