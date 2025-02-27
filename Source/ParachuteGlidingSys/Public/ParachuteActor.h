
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParachuteActor.generated.h"

class USkeletalMeshComponent;
class UAnimationAsset;

UCLASS()
class PARACHUTEGLIDINGSYS_API AParachuteActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AParachuteActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Parachute", meta = (AllowPrivateAccess = "true"))
	void ParachuteIdle();

	UFUNCTION(BlueprintCallable, Category = "Parachute", meta = (AllowPrivateAccess = "true"))
	void ParachuteDeploy();

	UFUNCTION(BlueprintCallable, Category = "Parachute", meta = (AllowPrivateAccess = "true"))
	void ParachuteLand();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ParachuteMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimationAsset* DeployAnimation = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimationAsset* IdleAnimation = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimationAsset* LandAnimation = nullptr;

};
