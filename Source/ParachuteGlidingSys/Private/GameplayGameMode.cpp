
#include "GameplayGameMode.h"
#include "PlayerCharacter.h"
#include "GameplayPlayerController.h"

AGameplayGameMode::AGameplayGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = AGameplayPlayerController::StaticClass();
}

void AGameplayGameMode::ChangeDefaultPawnClass(TSubclassOf<APawn> NewPawnClass)
{
	if (NewPawnClass)
	{
		DefaultPawnClass = NewPawnClass;
	}
}


