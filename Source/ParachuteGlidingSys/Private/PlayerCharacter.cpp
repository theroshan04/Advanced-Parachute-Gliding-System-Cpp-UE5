
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "ParachuteActor.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;

	PlayerCapsule = GetCapsuleComponent();
	PlayerMesh = GetMesh();
	PlayerMovementComponent = GetCharacterMovement();

	PlayerCapsule->SetCapsuleHalfHeight(90.0f);
	PlayerCapsule->SetCapsuleRadius(40.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(PlayerCapsule);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->SetRelativeLocation(FVector(0.0f, 60.0f, 40.0f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	PlayerMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
	PlayerMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	PlayerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ParachuteReferencePoint = CreateDefaultSubobject<USceneComponent>(TEXT("ParachuteReferencePoint"));
	ParachuteReferencePoint->SetupAttachment(PlayerMesh);

	BagpackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bagpack"));
	BagpackMesh->SetupAttachment(PlayerMesh, TEXT("BagpackSocket"));

	PrimaryWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrimaryWeapon"));
	PrimaryWeaponMesh->SetupAttachment(PlayerMesh, TEXT("PrimaryWeaponSocket"));

	SecondaryWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondaryWeapon"));
	SecondaryWeaponMesh->SetupAttachment(PlayerMesh, TEXT("SecondaryWeaponSocket"));

	PlayerMovementComponent->GravityScale = 0.5f;
	PlayerMovementComponent->AirControl = 1.0f;
	PlayerMovementComponent->FallingLateralFriction = 0.2f;
	PlayerMovementComponent->MaxWalkSpeed = 600.0f;

	CurrentDistance = 0.0f;
	ParachuteFloatDownSpeed = -1000.0f;
	ParachuteSpeedSlow = 600.0f;
	SkydiveSpeedHorizontal = 3000.0f;
	ParachuteDistanceMinimum = 1000.0f;
	LowestDistance = 9000.0f;
	bIsSkydiving = true;
	bShootPulled = false;
	bStopEventTick = false;
	bCancelParachute = true;
	bCanTeleport = true;
	bAutoOpenAllowed = true;

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Default, 0);
		}
	}

	// UpdateCameraArmLengthTimelineBinding
	FOnTimelineFloat ProgressFunction;
	ProgressFunction.BindUFunction(this, TEXT("UpdateCameraArmLength"));
	UpdateCameraArmLengthTimeline.AddInterpFloat(FloatCurve, ProgressFunction);
	UpdateCameraArmLengthTimeline.SetLooping(false);
	UpdateCameraArmLengthTimeline.SetPlayRate(1.0f);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bStopEventTick)
	{
		ParachuteDistanceToGround();
		SetSkydive();
	}

	UpdateCameraArmLengthTimeline.TickTimeline(DeltaTime);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookVector.X);
	AddControllerPitchInput(LookVector.Y);
}

void APlayerCharacter::UpdateCameraArmLength(float Value)
{
	SpringArm->TargetArmLength = FMath::Lerp(1000.0f, 500.0f, Value);
}

void APlayerCharacter::PossessPlayer(FVector Location, APawn* Pawn)
{
	SetActorLocation(Location);
	PlayerController->Possess(Pawn);
}

void APlayerCharacter::SetSkydive()
{
	if (PlayerMovementComponent->IsFalling())
	{
		if (bShootPulled)
		{
			FVector LastVelocity = PlayerMovementComponent->GetLastUpdateVelocity();
			LastVelocity.Z = ParachuteFloatDownSpeed;
			PlayerMovementComponent->Velocity = LastVelocity;
			PlayerMovementComponent->MaxWalkSpeed = ParachuteSpeedSlow;
		}
		else
		{
			if (CurrentDistance >= ParachuteDistanceMinimum)
			{
				bIsSkydiving = true;
				PlayerMovementComponent->MaxWalkSpeed = SkydiveSpeedHorizontal;
			}
		}
	}
	else
	{
		if (bShootPulled)
		{
			ParachuteLand();
		}
		else
		{
			if (bIsSkydiving)
			{
				PlayerMovementComponent->MaxWalkSpeed = 600.0f;
				bIsSkydiving = false;
				SkydiveCrash();
			}
		}
	}
}

void APlayerCharacter::DeployParachute()
{
	if (bIsSkydiving)
	{
		if (bShootPulled)
		{
			if (bCancelParachute && CurrentDistance >= LowestDistance)
			{
				bIsSkydiving = true;
				bShootPulled = false;
				Parachute->Destroy();
			}
		}
		else
		{
			bShootPulled = true;
			bCanTeleport = false;
			FTransform ParachuteSpawnTransform = PlayerMesh->GetSocketTransform(TEXT("ParachuteSocket"));
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Parachute = GetWorld()->SpawnActor<AParachuteActor>(ParachuteClass, ParachuteSpawnTransform, SpawnParams);
			Parachute->AttachToComponent(PlayerMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("ParachuteSocket"));
			Parachute->ParachuteDeploy();
			UpdateCameraArmLengthTimeline.ReverseFromEnd();
			ParachuteDeployCooldown();
		}

	}
}

void APlayerCharacter::ParachuteLand()
{
	bShootPulled = false;
	bIsSkydiving = false;
	Parachute->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Parachute->ParachuteLand();
	PlayerLandReset();
}

void APlayerCharacter::AutoDeploy()
{
	if (bIsSkydiving && !bShootPulled)
	{
		if (bAutoOpenAllowed)
		{
			DeployParachute();
		}
	}
}

void APlayerCharacter::ParachuteDistanceToGround()
{
	FVector StartLocation = ParachuteReferencePoint->GetComponentLocation();
	FVector EndLocation = StartLocation + (ParachuteReferencePoint->GetUpVector() * FVector(0.0f, 0.0f, -100000.0f));
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, QueryParams);
	if (bHit)
	{
		CurrentDistance = HitResult.Distance;
		if (bIsSkydiving && !bShootPulled && CurrentDistance <= LowestDistance && CurrentDistance >= ParachuteDistanceMinimum)
		{
			AutoDeploy();
		}
	}
}

void APlayerCharacter::ParachuteDeployCooldown()
{
	bCancelParachute = false;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			bCancelParachute = true;
			bCanTeleport = true;
		}, 0.2f, false);
}

void APlayerCharacter::PlayerLandReset()
{
	UGameplayStatics::PlaySoundAtLocation(this, JumpSound, GetActorLocation());
	bUseControllerRotationYaw = true;
	PlayerMovementComponent->MaxWalkSpeed = 600.0f;
	DisableInput(PlayerController);
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			bStopEventTick = true;
			EnableInput(PlayerController);
			PlayerMovementComponent->GravityScale = 1.0f;
			UpdateCameraArmLengthTimeline.PlayFromStart();
		}, 1.333f, false);
}

//Incomplete
void APlayerCharacter::SkydiveCrash()
{
	DisableInput(PlayerController);
	PlayerMesh->SetSimulatePhysics(true);
}

// Input Binding
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	}
}

