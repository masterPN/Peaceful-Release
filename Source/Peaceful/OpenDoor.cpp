// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "GameFramework/InputSettings.h"


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
	// AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();

	TArray<UAudioComponent*> AudioComponent;
	GetOwner()->GetComponents<UAudioComponent>(OUT AudioComponent);

	for(UAudioComponent* Component : AudioComponent){
		if(Component->GetName()=="OpenDoorAudio"){
			DoorOpenAudioComponent = Component;
		}else if(Component->GetName()=="CloseDoorAudio"){
			DoorCloseAudioComponent = Component;
		}else if(Component->GetName()=="LockedDoorAudio"){
			DoorLockedAudioComponent = Component;
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
			if(GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::E) && IsLock==false){
				OpenDoor(DeltaTime);
				DoorLastOpened = GetWorld()->GetTimeSeconds();
				isOpening = true;
			}else if(isOpening){
				OpenDoor(DeltaTime);
				DoorLastOpened = GetWorld()->GetTimeSeconds();
			}else if(GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::E) && IsLock==true){
				if(DoorLockedAudioComponent && !DoorLockedAudioComponent->IsPlaying()){
					DoorLockedAudioComponent->Play();
				}
			}
		}
	}

	// no actor obstruct door, close it
	if(!isActorObstruct){
		if(GetWorld()->GetTimeSeconds()-DoorLastOpened>DoorCloseDelay){
			CloseDoor(DeltaTime);
			isOpening = false;
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime){
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime*DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	CloseDoorSound = false;
	if(!DoorOpenAudioComponent){
		return;
	}
	if(!OpenDoorSound){
		DoorOpenAudioComponent->Play();
		OpenDoorSound = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime){
	CurrentYaw = FMath::Lerp(CurrentYaw, InitYaw, DeltaTime*DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);

	OpenDoorSound = false;
	if(!DoorCloseAudioComponent){
		return;
	}
	if(!CloseDoorSound){
		DoorCloseAudioComponent->Play();
		CloseDoorSound = true;
	}
}