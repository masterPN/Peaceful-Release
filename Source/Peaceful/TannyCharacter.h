// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TannyCharacter.generated.h"

UCLASS()
class PEACEFUL_API ATannyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATannyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float MoveSpeed = 0.2;

	bool isRun = false;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void MoveRun();
	void MoveWalk();

	UPROPERTY(EditAnywhere)
	float MaxRange = 1000;

	bool isInCover = false;

	void CoverObject();
	void StartCover(FVector HitNormal);
	void StopCover();
};
