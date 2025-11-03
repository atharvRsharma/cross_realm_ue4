#include "JsonGameMode.h"
#include "GlowingOrb.h" 

#include "Json.h"
#include "JsonUtilities.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void AJsonGameMode::BeginPlay()
{
	Super::BeginPlay();

	FOrbStateData LoadedData;

	if (LoadOrbState(LoadedData))
	{

		FVector UE_Position = FVector(LoadedData.position.X, LoadedData.position.Z, LoadedData.position.Y) * 100.0f;
		FVector UE_Velocity = FVector(LoadedData.velocity.X, LoadedData.velocity.Z, LoadedData.velocity.Y) * 100.0f;
		float UE_Energy = LoadedData.energy;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AGlowingOrb* SpawnedOrb = GetWorld()->SpawnActor<AGlowingOrb>(
			OrbClassToSpawn,
			UE_Position,
			FRotator::ZeroRotator,
			SpawnParams
		);

		if (SpawnedOrb)
		{
			SpawnedOrb->InitializeState(UE_Position, UE_Velocity, UE_Energy);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AJsonGameMode: 'entity_state.json' not found or failed to parse. No orb spawned."));
	}
}

bool AJsonGameMode::LoadOrbState(FOrbStateData& OutData)
{
	FString FilePath = TEXT("C:/Programming/task/entity_state.json");

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("AJsonGameMode: Failed to load file: %s"), *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject;

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("AJsonGameMode: Failed to parse JSON string. Check for syntax errors."));
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* PositionArray = nullptr;
	const TArray<TSharedPtr<FJsonValue>>* VelocityArray = nullptr;

	double EnergyAsDouble;

	if (!JsonObject->TryGetNumberField(TEXT("energy"), EnergyAsDouble) ||
		!JsonObject->TryGetStringField(TEXT("id"), OutData.id) ||
		!JsonObject->TryGetStringField(TEXT("state"), OutData.state) ||
		!JsonObject->TryGetArrayField(TEXT("position"), PositionArray) ||
		!JsonObject->TryGetArrayField(TEXT("velocity"), VelocityArray))
	{
		UE_LOG(LogTemp, Error, TEXT("AJsonGameMode: A required field (energy, id, state, position, or velocity) is missing from the JSON."));
		return false;
	}

	OutData.energy = static_cast<float>(EnergyAsDouble);

	if (PositionArray == nullptr || VelocityArray == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AJsonGameMode: 'position' or 'velocity' array is null."));
		return false;
	}

	if (PositionArray->Num() == 3 && VelocityArray->Num() == 3)
	{
		OutData.position.X = (*PositionArray)[0]->AsNumber();
		OutData.position.Y = (*PositionArray)[1]->AsNumber();
		OutData.position.Z = (*PositionArray)[2]->AsNumber();

		OutData.velocity.X = (*VelocityArray)[0]->AsNumber();
		OutData.velocity.Y = (*VelocityArray)[1]->AsNumber();
		OutData.velocity.Z = (*VelocityArray)[2]->AsNumber();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AJsonGameMode: 'position' or 'velocity' array is not size 3."));
		return false;
	}

	return true;
}