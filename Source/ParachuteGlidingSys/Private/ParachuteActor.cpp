
#include "ParachuteActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"


AParachuteActor::AParachuteActor()
{
	PrimaryActorTick.bCanEverTick = true;

	ParachuteMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	ParachuteMesh->SetupAttachment(RootComponent);

}

void AParachuteActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AParachuteActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AParachuteActor::ParachuteIdle()
{
	ParachuteMesh->PlayAnimation(IdleAnimation, true);
}

void AParachuteActor::ParachuteDeploy()
{
	ParachuteMesh->SetVisibility(true, false);
	ParachuteMesh->PlayAnimation(DeployAnimation, false);

	float DeployAnimationDuration = DeployAnimation->GetPlayLength();
	FTimerHandle DeployAnimationTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DeployAnimationTimerHandle, this, &AParachuteActor::ParachuteIdle, DeployAnimationDuration, false);

}

void AParachuteActor::ParachuteLand()
{
	ParachuteMesh->PlayAnimation(LandAnimation, false);
	SetLifeSpan(20.0f);
}

