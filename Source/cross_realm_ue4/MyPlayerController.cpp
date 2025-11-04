#include "MyPlayerController.h"
#include "JsonGameMode.h" 
#include "Kismet/GameplayStatics.h"

AMyPlayerController::AMyPlayerController()
{
	bEnableClickEvents = false;
	bShowMouseCursor = true;
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent(); 

	InputComponent->BindAction("Reload", IE_Pressed, this, &AMyPlayerController::OnReloadPressed);
}

void AMyPlayerController::OnReloadPressed()
{
	UE_LOG(LogTemp, Log, TEXT("Reload key pressed!"));

	AJsonGameMode* GameMode = Cast<AJsonGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode != nullptr)
	{
		GameMode->SpawnOrUpdateOrb();
	}
}