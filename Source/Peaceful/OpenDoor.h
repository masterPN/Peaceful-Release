// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "OpenDoor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PEACEFUL_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void OpenDoor(float DeltaTime);
	void CloseDoor(float DeltaTime);

	void FindPressurePlate();
	void FindAudioComp();

	// Track if sound has been played.
	bool OpenDoorSound = false;
	bool CloseDoorSound = true;

private:

	float InitYaw;
	float CurrentYaw;

	UPROPERTY(EditAnywhere)
	FString DoorName;

	UPROPERTY(EditAnywhere)
	bool IsLock = false;

	UPROPERTY(EditAnywhere)
	float OpenAngle = 90.f;

	float DoorLastOpened = 0.f;

	UPROPERTY(EditAnywhere)
	float DoorCloseDelay = .5f;

	UPROPERTY(EditAnywhere)
	float DoorOpenSpeed = .8f;

	UPROPERTY(EditAnywhere)
	float DoorCloseSpeed = 2.f;

	bool isActorObstruct = false;

	bool isOpening = false;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* OpenArea = nullptr;

	UPROPERTY()
	UAudioComponent* DoorOpenAudioComponent = nullptr;

	UPROPERTY()
	UAudioComponent* DoorCloseAudioComponent = nullptr;
		
};
