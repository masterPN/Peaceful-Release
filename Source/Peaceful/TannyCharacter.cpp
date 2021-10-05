// Fill out your copyright notice in the Description page of Project Settings.


#include "TannyCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	//Set up crouch
	PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &ATannyCharacter::CoverObject);

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

void ATannyCharacter::CoverObject() 
{
	// Get camera view point
	AController* OwnerController = GetController();
	if(OwnerController==nullptr){
		return;
	}

	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	// DrawDebugCamera(GetWorld(), Location, Rotation, 90, 2.0f, FColor::Green, true);

	FVector End = Location+(Rotation.Vector()*MaxRange);
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
	if(bSuccess){
		if(isInCover){
			UE_LOG(LogTemp, Warning, TEXT("stop cover"));
			StopCover();
			UE_LOG(LogTemp, Warning, TEXT("Some warning message %s"), (GetCharacterMovement()->IsCrouching() ? TEXT("true") : TEXT("false")));
		}else{
			UE_LOG(LogTemp, Warning, TEXT("start cover"));
			StartCover(Hit.Normal);
			UE_LOG(LogTemp, Warning, TEXT("Some warning message %s"), (GetCharacterMovement()->IsCrouching() ? TEXT("true") : TEXT("false")));
		}
	}
}

void ATannyCharacter::StartCover(FVector HitNormal)
{
	GetCharacterMovement()->SetPlaneConstraintEnabled(true);
	GetCharacterMovement()->SetPlaneConstraintNormal(HitNormal);
	// GetCharacterMovement()->Crouch();
	Crouch();
	bUseControllerRotationYaw = false;

	isInCover = true;
}

void ATannyCharacter::StopCover()
{
	GetCharacterMovement()->SetPlaneConstraintEnabled(false);
	// GetCharacterMovement()->UnCrouch();
	UnCrouch();
	bUseControllerRotationYaw = true;

	isInCover = false;
}