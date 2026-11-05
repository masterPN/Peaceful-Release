// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/PlayerController.h"
#include "Math/MathFwd.h"
#include "Math/UnrealMathUtility.h"
#include "TannyCharacter.h"


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	InitYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitYaw;
	OpenAngle += InitYaw;

	FindPressurePlate();
	FindAudioComp();

}

void UOpenDoor::FindPressurePlate(){
	if(!OpenArea){
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component, but no OpenArea set!"), *GetOwner()->GetName());
	}
}

void UOpenDoor::FindAudioComp(){
	TArray<UAudioComponent*> AudioComponents;
	GetOwner()->GetComponents<UAudioComponent>(OUT AudioComponents);

	for(UAudioComponent* Component : AudioComponents){
		if(Component->GetName()=="OpenDoorAudio"){
			DoorOpenAudioComponent = Component;
		}else if(Component->GetName()=="CloseDoorAudio"){
			DoorCloseAudioComponent = Component;
		}
	}

	if(!DoorOpenAudioComponent){
		UE_LOG(LogTemp, Error, TEXT("%s Missing door open audio component"), *GetOwner()->GetName());
	}
	if(!DoorCloseAudioComponent){
		UE_LOG(LogTemp, Error, TEXT("%s Missing door close audio component"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsAnimating) {
		StepAnimation(DeltaTime);
	}

	isActorObstruct = false;

	if(!OpenArea){
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("isLock %s"), ( IsLock ? TEXT("true") : TEXT("false") ));

	TArray<AActor*> OverlappingActors;
	OpenArea->GetOverlappingActors(OUT OverlappingActors);

	for(AActor* Actor : OverlappingActors){
		if(Actor->GetName() == "BP_TannyCharacter_C_0"){
			isActorObstruct = true;
			if(GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::E) && IsLock==false && !bIsAnimating){
				StartAnimating(true);
				DoorLastOpened = GetWorld()->GetTimeSeconds();
				isOpening = true;
			}else if(isOpening && !bIsAnimating){
				StartAnimating(true);
				DoorLastOpened = GetWorld()->GetTimeSeconds();
			}
		}
	}

	// no actor obstruct door, close it
	if(!isActorObstruct){
		if(GetWorld()->GetTimeSeconds()-DoorLastOpened>DoorCloseDelay){
			StartAnimating(false);
			isOpening = false;
		}
	}
}

void UOpenDoor::Interact(ATannyCharacter *Interactor)
{
	const bool bUnlocked = !IsLock || (Interactor && Interactor->HasKey(RequiredKeyId));

	if (bUnlocked && !bIsAnimating) {
		StartAnimating(true);
	}
}

void UOpenDoor::StartAnimating(bool bOpen)
{
	bOpenTarget = bOpen;
	bIsAnimating = true;
	DoorLastOpened = GetWorld()->GetTimeSeconds();
	SetComponentTickEnabled(true);

	if (bOpen) {
		CloseDoorSound = false;
		if (DoorOpenAudioComponent && !OpenDoorSound) {
			DoorOpenAudioComponent->Play();
			OpenDoorSound = true;
		}
	} else {
		OpenDoorSound = false;
		if (DoorCloseAudioComponent && !CloseDoorSound) {
			DoorCloseAudioComponent->Play();
			CloseDoorSound = true;
		}
	}
}

void UOpenDoor::StepAnimation(float DeltaTime)
{
	const float Target = bOpenTarget ? OpenAngle : InitYaw;
	const float Speed = bOpenTarget ? DoorOpenSpeed : DoorCloseSpeed;

	CurrentYaw = FMath::Lerp(CurrentYaw, Target, DeltaTime*Speed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	if (FMath::IsNearlyEqual(CurrentYaw, Target, .5f)) {
		bIsAnimating = false;
		if (!bOpenTarget) {
			SetComponentTickEnabled(false);
		}
	}
}
