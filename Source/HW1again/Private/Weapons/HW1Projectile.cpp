#include "Weapons/HW1Projectile.h"

AHW1Projectile::AHW1Projectile()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AHW1Projectile::BeginPlay()
{
	Super::BeginPlay();
}

void AHW1Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
