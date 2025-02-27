
#include "CarryplanePawn.h"
#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

ACarryplanePawn::ACarryplanePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	PlaneMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("PlaneMesh"));
	PlaneMesh->SetupAttachment(RootComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(PlaneMesh);
	SpringArm->TargetArmLength = 1000.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetupAttachment(PlaneMesh);
	PlayerMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

	PlayerSpawnLocator = CreateDefaultSubobject<USceneComponent>(TEXT("PlayerSpawnLocator"));
	PlayerSpawnLocator->SetupAttachment(PlaneMesh);
}

void ACarryplanePawn::BeginPlay()
{
	Super::BeginPlay();
	PlayCameraShake();
	EngineSoundComponent = UGameplayStatics::CreateSound2D(this, EngineSound);
	EngineSoundComponent->Play();
}

void ACarryplanePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float LastBoneRotationRoll = PlaneMesh->GetBoneRotationByName(TEXT("Propellor_J"), EBoneSpaces::ComponentSpace).Roll;
	PlaneMesh->SetBoneRotationByName(TEXT("Propellor_J"), FRotator(0.0f, 0.0f, LastBoneRotationRoll + 40.0f), EBoneSpaces::ComponentSpace);
}

void ACarryplanePawn::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ACarryplanePawn::Jump()
{
	EngineSoundComponent->Stop();
	GetWorld()->GetTimerManager().ClearTimer(CameraShakeTimerHandle);
	CameraShakeTimerHandle.Invalidate();
	UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation());
	SpawnAndPossessPlayer();
}

void ACarryplanePawn::SpawnAndPossessPlayer()
{
	FActorSpawnParameters SpawnParams;
	FTransform SpawnTransform = PlayerSpawnLocator->GetComponentTransform();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APlayerCharacter* PlayerCharacter = GetWorld()->SpawnActor<APlayerCharacter>(PlayerCharacterClass, SpawnTransform, SpawnParams);
	GetController()->Possess(PlayerCharacter);
}

void ACarryplanePawn::PlayCameraShake()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		APlayerController* LocalController = PlayerController;
		GetWorld()->GetTimerManager().SetTimer(CameraShakeTimerHandle, [LocalController,this]()
			{
				LocalController->ClientStartCameraShake(CameraShakeClass, 0.5f);
			}, 1.0f, true);	
	}
}

//InputBinding
void ACarryplanePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Default, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACarryplanePawn::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACarryplanePawn::Jump);
	}
}

