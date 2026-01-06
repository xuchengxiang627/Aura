// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumMinions;

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	TArray<FVector> SpawnLocations;
	for (int i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector SpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation + FVector(0.f, 0.f, 400.f), SpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
		if (HitResult.bBlockingHit)
		{
			SpawnLocation = HitResult.ImpactPoint;
		} // 确保点位在地面上

		SpawnLocations.Add(SpawnLocation);
	}

	// UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Location, Location + LeftOfSpread * MaxSpawnDistance, 5.f, FLinearColor::Red, 3.f);
	//
	// const FVector RightOfSpread = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	// UKismetSystemLibrary::DrawDebugArrow(GetWorld(), Location, Location + RightOfSpread * MaxSpawnDistance, 5.f, FLinearColor::Red, 3.f);
	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinioClass()
{
	return MinionClasses[FMath::RandRange(0, MinionClasses.Num() - 1)];
}
