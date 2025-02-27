
#include "PlayerAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	if (!(TryGetPawnOwner())) return;
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (!PlayerCharacter) return;
	FVector PlayerVelocity = PlayerCharacter->GetVelocity();
	FRotator PlayerRotation = PlayerCharacter->GetActorRotation();
	Speed = PlayerVelocity.Size();
	Direction = CalculateDirection(PlayerVelocity, PlayerRotation);
	ForwardSpeed = PlayerRotation.UnrotateVector(PlayerVelocity).X;
	SidewaysSpeed = PlayerRotation.UnrotateVector(PlayerVelocity).Y;
	bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();
	bShootPulled = PlayerCharacter->bShootPulled;
	bIsSkyDiving = PlayerCharacter->bIsSkydiving;
}
