// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
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

bool UCoverComponent::CoverTrace(bool bIsRightSide) const
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if(!OwnerChar){
		return false;
	}

	UCharacterMovementComponent* Movement = OwnerChar->GetCharacterMovement();
	if(!Movement){
		return false;
	}

	FVector Offset = Movement->GetPlaneConstraintNormal();
	if(bIsRightSide){
		Offset *= -1.f;
	}

	FRotator OffsetRotX = UKismetMathLibrary::MakeRotFromX(Offset);
	FVector Lateral = UKismetMathLibrary::GetRightVector(OffsetRotX);

	FVector Start = OwnerChar->GetActorLocation()+(Lateral*CapsuleSize);
	FVector End = bIsRightSide
		? Start+(Offset*200.f)
		: Start-(Offset*200.f);

	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
}

bool UCoverComponent::ApplyCoverMovement(float AxisValue)
{
	if(!bInCover){
		return false;
	}

	ACharacter *OwnerChar = Cast<ACharacter>(GetOwner());
	if(!OwnerChar){
		return false;
	}

	UCharacterMovementComponent *Movement = OwnerChar->GetCharacterMovement();
	if(!Movement){
		return false;
	}

	bRightHit = CoverTrace(true);
	bLeftHit = CoverTrace(false);

	FRotator Rotation = OwnerChar->GetControlRotation();
	FRotator RotationYaw = UKismetMathLibrary::MakeRotator(0, 0, Rotation.Yaw);
	FVector RightVector = UKismetMathLibrary::GetRightVector(RotationYaw);

	if(bRightHit && bLeftHit){
		// Slide along the wall, refreshing the plane constraint so corners are followed.
		FVector Start = OwnerChar->GetActorLocation();
		FVector Distance = Movement->GetPlaneConstraintNormal()*-200.f;
		FVector End = Start + Distance;

		FHitResult Hit;
		FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
		bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

		if(bSuccess){
			Movement->SetPlaneConstraintNormal(Hit.Normal);
			OwnerChar->AddMovementInput(RightVector, AxisValue);
		}
	}else{
		// Block movement that would push past an open edge.
		if(FMath::IsNearlyEqual(AxisValue, 1.f)){
			if(bRightHit){
				OwnerChar->AddMovementInput(RightVector, AxisValue);
			}
		}else if(FMath::IsNearlyEqual(AxisValue, -1.f)){
			if(bLeftHit){
				OwnerChar->AddMovementInput(RightVector, AxisValue);
			}
		}
	}

	return true;
}
