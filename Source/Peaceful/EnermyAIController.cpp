// Fill out your copyright notice in the Description page of Project Settings.


#include "EnermyAIController.h"
#include "CoreGlobals.h"
#include "HAL/Platform.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Logging/LogMacros.h"
#include "Logging/LogVerbosity.h"


void AEnermyAIController::BeginPlay()
{
    Super::BeginPlay();

    if(AIBehavior == nullptr){
        UE_LOG(LogTemp, Error, TEXT("%s has no AIBehavior"), *GetName());
        return;
    }

    RunBehaviorTree(AIBehavior);

    APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
    GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
    
}

void AEnermyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if(LineOfSightTo(PlayerPawn)){
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
        GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());

    }else{
        GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
    }
}
