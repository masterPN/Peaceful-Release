// Fill out your copyright notice in the Description page of Project Settings.


#include "EnermyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"


void AEnermyAIController::BeginPlay()
{
    Super::BeginPlay();

    if(AIBehavior != nullptr){
        RunBehaviorTree(AIBehavior);

        APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
        GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
    }
    
}

void AEnermyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    APawn *PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if(LineOfSightTo(PlayerPawn)){
        GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
        GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), GetPawn()->GetActorLocation());

    }else{
        GetBlackboardComponent()->ClearValue(TEXT("PlayerLocation"));
    }
}
