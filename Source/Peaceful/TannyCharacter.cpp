// Fill out your copyright notice in the Description page of Project Settings.


#include "TannyCharacter.h"
#include "Components/InputComponent.h"
#include "CoverComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/MovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "HAL/Platform.h"
#include "Kismet/KismetMathLibrary.h"

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

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation+PlayerViewPointRotation.Vector()*ReachRange;

	// DrawDebugLine(GetWorld(), PlayerViewPointLocation, LineTraceEnd, FColor(255, 0, 0), false, -1.f, 0, 5.f);

	CoverComponent = CreateDefaultSubobject<UCoverComponent>(TEXT("CoverComponent"));

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
	PlayerInputComponent->BindAction(TEXT("Interaction"), EInputEvent::IE_Pressed, this, &ATannyCharacter::InteractionTrigger);
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
	bool isCrouch = GetCharacterMovement()->IsCrouching();
	
	if(isInCover){
		// Cover Trace
		isRightHit = CoverTrace(true);
		isLeftHit = CoverTrace(false);
		
		UE_LOG(LogTemp, Warning, TEXT("right is %s"), ( isRightHit ? TEXT("true") : TEXT("false") ));
		UE_LOG(LogTemp, Warning, TEXT("left is %s"), ( isLeftHit ? TEXT("true") : TEXT("false") ));

		if(isRightHit && isLeftHit){
			FVector Start = GetActorLocation();
			FVector distance = UKismetMathLibrary::Multiply_VectorFloat(GetCharacterMovement()->GetPlaneConstraintNormal(), -200);
			FVector End = Start + distance;

			FHitResult Hit;
			FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
			bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

			if(bSuccess){
				GetCharacterMovement()->SetPlaneConstraintNormal(Hit.Normal);

				FRotator Rotation = GetControlRotation();
				FRotator RotationYaw = UKismetMathLibrary::MakeRotator(0, 0, Rotation.Yaw);
				FVector RightVector = UKismetMathLibrary::GetRightVector(RotationYaw);
				AddMovementInput(RightVector*AxisValue);
			}
		}else{
			FRotator Rotation = GetControlRotation();
			FRotator RotationYaw = UKismetMathLibrary::MakeRotator(0, 0, Rotation.Yaw);
			FVector RightVector = UKismetMathLibrary::GetRightVector(RotationYaw);

			if(AxisValue==1.0f){
				if(isRightHit && AxisValue!=0){
					AddMovementInput(RightVector*AxisValue);
				}
			}

		}
	}else{
		if(isRun){
			AddMovementInput(GetActorRightVector()*AxisValue);
		}else{
			AxisValue = AxisValue/2;
			AddMovementInput(GetActorRightVector()*AxisValue);	
		}
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

void ATannyCharacter::CrouchToggle(){
	if(!(GetCharacterMovement()->IsCrouching())){
		Crouch();
	}else{
		GetCharacterMovement()->SetPlaneConstraintEnabled(false);
		UnCrouch();
		bUseControllerRotationYaw = true;

		isInCover = false;
	}
}

bool ATannyCharacter::CoverTrace(bool isRightSide)
{
	AController* OwnerController = GetController();
	if(OwnerController==nullptr){
		return NULL;
	}

	FVector Location;
	FRotator Rotation;
	// OwnerController->GetPlayerViewPoint(Location, Rotation);
	Location = GetActorLocation();
	Rotation = GetActorRotation();

	FVector distance;
	if(isRightSide){
		distance = UKismetMathLibrary::Multiply_VectorFloat(GetCharacterMovement()->GetPlaneConstraintNormal(), -1);
	}else{
		distance = (GetCharacterMovement()->GetPlaneConstraintNormal());
	}
	FRotator distanceRotX = UKismetMathLibrary::MakeRotFromX(distance);
	FVector distanceRight = UKismetMathLibrary::GetRightVector(distanceRotX);

	FVector Start = UKismetMathLibrary::Add_VectorVector(Location, UKismetMathLibrary::Multiply_VectorFloat(distanceRight, CapsuleSize));
	FVector End;
	if(isRightSide){
		End = UKismetMathLibrary::Add_VectorVector(Start, UKismetMathLibrary::Multiply_VectorFloat(distance, 200));
	}else{
		End = UKismetMathLibrary::Add_VectorVector(Start, UKismetMathLibrary::Multiply_VectorFloat(distance, -200));
	}

	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

	if(bSuccess){
		// DrawDebugPoint(GetWorld(), Hit.Location, 20, FColor::Red, true);
	}

	return bSuccess;
}

// Interaction

void ATannyCharacter::InteractionTrigger()
{
	// AController* OwnerController = GetController();
	// if(OwnerController==nullptr){
	// 	return;
	// }

	// FVector Location;
	// FRotator Rotation;
	// Location = GetActorLocation();
	// Rotation = GetActorRotation();


	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToInteract = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	if(ActorHit){
		UE_LOG(LogTemp, Warning, TEXT("%s"), *ActorHit->GetName());

		// update item on the character
		if(ActorHit->GetName()=="KeySandbox"){
			KeySandbox = true;
		}

		ActorHit->SetActorHiddenInGame(true);
	}

	UE_LOG(LogTemp, Warning, TEXT("The boolean value is %s"), ( KeySandbox ? TEXT("true") : TEXT("false") ));


}

FHitResult ATannyCharacter::GetFirstPhysicsBodyInReach() const
{
	FHitResult Hit;
	// Ray-cast out to a certain distance (Reach)
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_GameTraceChannel2),
		TraceParams
	);
	return Hit;
}

FVector ATannyCharacter::GetPlayersWorldPos() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

FVector ATannyCharacter::GetPlayersReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * ReachRange;
}