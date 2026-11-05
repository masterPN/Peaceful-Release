// Fill out your copyright notice in the Description page of Project Settings.


#include "TannyCharacter.h"
#include "Components/InputComponent.h"
#include "CoverComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "HAL/Platform.h"
#include "InteractionComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATannyCharacter::ATannyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CoverComponent = CreateDefaultSubobject<UCoverComponent>(TEXT("CoverComponent"));
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));

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

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation+PlayerViewPointRotation.Vector()*ReachRange;

	// DrawDebugLine(GetWorld(), PlayerViewPointLocation, LineTraceEnd, FColor(255, 0, 0), false, -1.f, 0, 5.f);
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

	// Set up crouch
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ATannyCharacter::CrouchToggle);
	PlayerInputComponent->BindAction(TEXT("Cover"), EInputEvent::IE_Pressed, CoverComponent, &UCoverComponent::TryToggleCover);

	// Set up interaction
	PlayerInputComponent->BindAction(TEXT("Interaction"), EInputEvent::IE_Pressed, InteractionComponent, &UInteractionComponent::TriggerInteraction);
}

void ATannyCharacter::MoveForward(float AxisValue) 
{
	if(isRun){
		AddMovementInput(GetActorForwardVector()*AxisValue);
	}else{
		AddMovementInput(GetActorForwardVector()*(AxisValue/2));
	}
}

void ATannyCharacter::MoveRight(float AxisValue) 
{
	if(CoverComponent->ApplyCoverMovement(AxisValue)){
		return;
	}

	if(isRun){
		AddMovementInput(GetActorRightVector()*AxisValue);
	}else{
		AddMovementInput(GetActorRightVector()*(AxisValue/2));	
	}
}

void ATannyCharacter::MoveRun() 
{
	isRun = true;
}

void ATannyCharacter::MoveWalk() 
{
	isRun = false;
}

void ATannyCharacter::CrouchToggle(){
	if(GetCharacterMovement()->IsCrouching()){
		GetCharacterMovement()->SetPlaneConstraintEnabled(false);
		UnCrouch();
		bUseControllerRotationYaw = true;
	}else{
		Crouch();
	}
}
