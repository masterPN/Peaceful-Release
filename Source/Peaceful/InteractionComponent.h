// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PEACEFUL_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:		
	void TriggerInteraction();

private:
	UPROPERTY(EditAnywhere)
	float ReachRange = 200.f;

	FHitResult GetFirstBodyInReach() const;
	FVector GetReachStart() const;
	FVector GetReachEnd() const;

		
};
