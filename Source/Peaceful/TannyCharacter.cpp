// Fill out your copyright notice in the Description page of Project Settings.


#include "TannyCharacter.h"

// Sets default values
ATannyCharacter::ATannyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATannyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATannyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATannyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up movement
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATannyCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATannyCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("MoveRun"), EInputEvent::IE_Pressed, this, &ATannyCharacter::MoveRun);
	PlayerInputComponent->BindAction(TEXT("MoveRun"), EInputEvent::IE_Released, this, &ATannyCharacter::MoveWalk);

	// Set up camera
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);

}

void ATannyCharacter::MoveForward(float AxisValue) 
{
	if(isRun){
		AddMovementInput(GetActorForwardVector()*AxisValue);
	}else{
		AxisValue = AxisValue/2;
		AddMovementInput(GetActorForwardVector()*AxisValue);
	}
}

void ATannyCharacter::MoveRight(float AxisValue) 
{
	if(isRun){
		AddMovementInput(GetActorRightVector()*AxisValue);
	}else{
		AxisValue = AxisValue/2;
		AddMovementInput(GetActorRightVector()*AxisValue);	
	}
}

void ATannyCharacter::MoveRun() 
{
	UE_LOG(LogTemp, Warning, TEXT("press shift"));
	isRun = true;
}

void ATannyCharacter::MoveWalk() 
{
	UE_LOG(LogTemp, Warning, TEXT("release shift"));
	isRun = false;
}

