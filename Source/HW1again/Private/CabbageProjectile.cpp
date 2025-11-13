// Fill out your copyright notice in the Description page of Project Settings.

#include "CabbageProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/Engine.h"

// Sets default values
ACabbageProjectile::ACabbageProjectile()
{
	// By default we don't need Tick for a projectile => disable for perf
	PrimaryActorTick.bCanEverTick = false;

	// Collision component (root)
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(8.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->SetNotifyRigidBodyCollision(true); // will receive OnComponentHit
	CollisionComp->SetGenerateOverlapEvents(false);
	RootComponent = CollisionComp;

	// Visual mesh (no collision)
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Projectile movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	InitialSpeed = 3000.f;
	MaxSpeed = 3000.f;
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// Damage defaults
	Damage = 20.0f;
	DamageType = UDamageType::StaticClass();

	// Destroy after a short time
	InitialLifeSpan = 4.0f;

	// Bind hit
	CollisionComp->OnComponentHit.AddDynamic(this, &ACabbageProjectile::OnHit);
}

// Called when the game starts or when spawned
void ACabbageProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Sync movement component values with properties (if changed in BP)
	if (ProjectileMovement)
	{
		ProjectileMovement->InitialSpeed = InitialSpeed;
		ProjectileMovement->MaxSpeed = MaxSpeed;
	}
}

// Called every frame (not used by default)
void ACabbageProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACabbageProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	AActor* MyOwner = GetOwner();
	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetVelocity().GetSafeNormal(), Hit,
			MyOwner ? MyOwner->GetInstigatorController() : nullptr, this, DamageType);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
		if (GEngine)
		{
			FString Msg = FString::Printf(TEXT("Projectile hit %s for %.1f damage"), *OtherActor->GetName(), Damage);
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, Msg);
		}
#endif

		Destroy();
	}
	else if (OtherComp && OtherComp->GetCollisionObjectType() == ECC_WorldStatic)
	{
		Destroy();
	}
}
