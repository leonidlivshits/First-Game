#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HW1Character.generated.h"

class AActor;
class USceneComponent;

UCLASS()
class HW1AGAIN_API AHW1Character : public ACharacter
{
	GENERATED_BODY()

public:
	AHW1Character();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Movement handlers
	void MoveForward(float Value);
	void MoveRight(float Value);

	// Fire handler
	UFUNCTION(BlueprintCallable)
	void OnFire();

	// Muzzle / spawn point for projectiles
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USceneComponent* MuzzleLocation;

	// Projectile class to spawn (set this in BP to BP_CabbageProjectile)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AActor> ProjectileClass;

	// Ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 Ammo;
};
