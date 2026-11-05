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

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
    GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
    
}

void AEnermyAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    UBlackboardComponent *BB = GetBlackboardComponent();
    if (!BB) {
        return;
    }

    if(LineOfSightTo(PlayerPawn)){
        BB->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
        BB->SetValueAsVector(TEXT("LastKnownPlayerLocation"), PlayerPawn->GetActorLocation());
    }else{
        BB->ClearValue(TEXT("PlayerLocation"));
    }
}
