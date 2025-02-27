
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class APlayerCharacter;

UCLASS()
class PARACHUTEGLIDINGSYS_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimationVariable", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimationVariable", meta = (AllowPrivateAccess = "true"))
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimationVariable", meta = (AllowPrivateAccess = "true"))
	float SidewaysSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimationVariable", meta = (AllowPrivateAccess = "true"))
	float ForwardSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimationVariable", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimationVariable", meta = (AllowPrivateAccess = "true"))
	bool bShootPulled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimationVariable", meta = (AllowPrivateAccess = "true"))
	bool bIsSkyDiving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AnimationVariable", meta = (AllowPrivateAccess = "true"))
	APlayerCharacter* PlayerCharacter;
	
};
