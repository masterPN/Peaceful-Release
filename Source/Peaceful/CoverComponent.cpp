// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Math/MathFwd.h"

// Sets default values for this component's properties
UCoverComponent::UCoverComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called every frame
void UCoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCoverComponent::TryToggleCover()
{
	ACharacter *OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) {
		return;
	}

	AController *OwnerController = OwnerChar->GetController();
	if (OwnerController == nullptr) {
		return;
	}

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	FVector End = Location+(Rotation.Vector()*MaxRange);
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

	if(bSuccess && !bInCover){
		StartCover(Hit.Normal);
	}else if(bInCover){
		StopCover();
	}

}

void UCoverComponent::StartCover(const FVector& HitNormal)
{
	ACharacter *OwnerChar = Cast<ACharacter>(GetOwner());
	if(!OwnerChar){
		return;
	}

	UCharacterMovementComponent *Movement = OwnerChar->GetCharacterMovement();
	if(!Movement){
		return;
	}

	Movement->SetPlaneConstraintEnabled(true);
	Movement->SetPlaneConstraintNormal(HitNormal);
	OwnerChar->Crouch();
	OwnerChar->bUseControllerRotationYaw = false;

	bInCover = true;
}

void UCoverComponent::StopCover()
{
	ACharacter *OwnerChar = Cast<ACharacter>(GetOwner());
	if(!OwnerChar){
		return;
	}

	UCharacterMovementComponent *Movement = OwnerChar->GetCharacterMovement();
	if(Movement){
		Movement->SetPlaneConstraintEnabled(false);
	}

	OwnerChar->UnCrouch();
	OwnerChar->bUseControllerRotationYaw = true;

	bInCover = false;
}