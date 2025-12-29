// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FIndNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FIndNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (APawn* OwingPawn = AIOwner->GetPawn())
	{
		// 如果有player标签，则要目标是寻找敌人
		const FName TargetTag = OwingPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

		TArray<AActor*> TargetActors;
		UGameplayStatics::GetAllActorsWithTag(OwingPawn->GetWorld(), TargetTag, TargetActors);

		float ClosestDistance = MAX_FLT;
		AActor* ClosestActor = nullptr;
		for (AActor* TargetActor : TargetActors)
		{
			// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Target Actor: %s"), *TargetActor->GetName()));
			if (TargetActor)
			{
				float Distance = OwingPawn->GetDistanceTo(TargetActor);
				// float Distance = FVector::Distance(OwingPawn->GetActorLocation(), TargetActor->GetActorLocation());
				if (Distance < ClosestDistance)
				{
					ClosestDistance = Distance;
					ClosestActor = TargetActor;
				}
			}
		}

		if (ClosestActor)
		{
			UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosestActor);
			UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector, ClosestDistance);
		}
	}

}
