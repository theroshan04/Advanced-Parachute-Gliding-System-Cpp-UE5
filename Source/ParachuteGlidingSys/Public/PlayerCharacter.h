
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCapsuleComponent;
class USceneComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class UCharacterMovementComponent;
class AParachuteActor;
class USoundBase;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class PARACHUTEGLIDINGSYS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	void PossessPlayer(FVector Location, APawn* Pawn);

protected:
	virtual void BeginPlay() override;
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void SetSkydive();
	void DeployParachute();
	void ParachuteLand();
	void AutoDeploy();
	void ParachuteDistanceToGround();
	void ParachuteDeployCooldown();
	void PlayerLandReset();
	void SkydiveCrash();
	UFUNCTION()
	void UpdateCameraArmLength(float Value);

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	USkeletalMeshComponent* PlayerMesh = nullptr;
	UCapsuleComponent* PlayerCapsule = nullptr;
	UCharacterMovementComponent* PlayerMovementComponent = nullptr;
	AParachuteActor* Parachute = nullptr;
	FTimeline UpdateCameraArmLengthTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attachment", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ParachuteReferencePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attachment", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BagpackMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attachment", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PrimaryWeaponMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attachment", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SecondaryWeaponMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Reference", meta = (AllowPrivateAccess = "true"))
	APlayerController* PlayerController = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AParachuteActor> ParachuteClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	USoundBase* JumpSound;

	UPROPERTY(EditAnywhere, Category = "Timeline", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* FloatCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* IMC_Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	float ParachuteFloatDownSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	float ParachuteSpeedSlow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	float SkydiveSpeedHorizontal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	float CurrentDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	float ParachuteDistanceMinimum;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	float LowestDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	bool bCancelParachute;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	bool bCanTeleport;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	bool bStopEventTick;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	bool bAutoOpenAllowed;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	bool bShootPulled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SkydiveVariable", meta = (AllowPrivateAccess = "true"))
	bool bIsSkydiving = true;
};
