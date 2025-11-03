#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "JsonGameMode.generated.h"

USTRUCT(BlueprintType)
struct FOrbStateData
{
	GENERATED_BODY()

	UPROPERTY() FVector position;
	UPROPERTY() FVector velocity;
	UPROPERTY() float energy;
	UPROPERTY() FString id;
	UPROPERTY() FString state;
};


UCLASS()
class CROSS_REALM_UE4_API AJsonGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	bool LoadOrbState(FOrbStateData& OutData);

	
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<class AGlowingOrb> OrbClassToSpawn;
};