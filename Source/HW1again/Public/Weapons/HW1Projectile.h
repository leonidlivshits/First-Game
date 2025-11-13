#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HW1Projectile.generated.h"

UCLASS()
class HW1AGAIN_API AHW1Projectile : public AActor
{
	GENERATED_BODY()

public:
	AHW1Projectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
