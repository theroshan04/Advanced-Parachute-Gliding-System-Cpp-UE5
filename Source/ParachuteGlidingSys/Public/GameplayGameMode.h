
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameplayGameMode.generated.h"


UCLASS()
class PARACHUTEGLIDINGSYS_API AGameplayGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameplayGameMode();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void ChangeDefaultPawnClass(TSubclassOf<APawn> NewPawnClass);
	
};
