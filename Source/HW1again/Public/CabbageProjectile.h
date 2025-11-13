// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CabbageProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UDamageType;

UCLASS()
class HW1AGAIN_API ACabbageProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACabbageProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

public:
	// Called every frame (we disable Tick by default, kept for extension)
	virtual void Tick(float DeltaTime) override;

	/** Collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	/** Visual mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* MeshComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovement;

	/** Damage inflicted on hit */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile|Damage")
	float Damage;

	/** Damage type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile|Damage")
	TSubclassOf<UDamageType> DamageType;

	/** Initial speed (fallback if not set in BP) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile|Movement")
	float InitialSpeed;

	/** Max speed (fallback if not set in BP) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile|Movement")
	float MaxSpeed;
};
