# create_player_projectile.ps1
# Usage:
# 1) Open PowerShell in project root (C:\UEProjects\HW1again)
# 2) Run: Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
# 3) Run: .\create_player_projectile.ps1

$root = "C:\UEProjects\HW1again"

# Paths
$publicChars = Join-Path $root "Source\HW1again\Public\Characters"
$privateChars = Join-Path $root "Source\HW1again\Private\Characters"
$publicWeap = Join-Path $root "Source\HW1again\Public\Weapons"
$privateWeap = Join-Path $root "Source\HW1again\Private\Weapons"
$configDir = Join-Path $root "Config"

# Create directories if missing
$dirs = @($publicChars, $privateChars, $publicWeap, $privateWeap, $configDir)
foreach ($d in $dirs) {
    if (-not (Test-Path $d)) { New-Item -ItemType Directory -Path $d -Force | Out-Null }
}

# HW1Character.h
$charH = @'
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HW1Character.generated.h"

class USceneComponent;
class AHW1Projectile;

UCLASS()
class HW1AGAIN_API AHW1Character : public ACharacter
{
    GENERATED_BODY()

public:
    AHW1Character();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
    // Stats
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    // Ammo / weapon
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    int32 MaxAmmo;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    int32 CurrentAmmo;

    // Currency
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency")
    int32 Credits;

    // Projectile class to spawn (set in editor or BP)
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AHW1Projectile> ProjectileClass;

    // Muzzle point (attach in BP or in constructor)
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    USceneComponent* FP_Muzzle;

    // Firing
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Fire();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void AddAmmo(int32 Amount);

    // Damage handling
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintCallable, Category = "Stats")
    void Die();
};
'@

$charH | Out-File -FilePath (Join-Path $publicChars "HW1Character.h") -Encoding UTF8 -Force

# HW1Character.cpp
$charCpp = @'
#include "HW1Character.h"
#include "HW1Projectile.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AHW1Character::AHW1Character()
{
    PrimaryActorTick.bCanEverTick = true;

    // Defaults
    MaxHealth = 100.f;
    CurrentHealth = MaxHealth;
    MaxAmmo = 30;
    CurrentAmmo = MaxAmmo;
    Credits = 0;

    // Create a muzzle scene component
    FP_Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("FP_Muzzle"));
    FP_Muzzle->SetupAttachment(GetRootComponent());
}

void AHW1Character::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    CurrentAmmo = MaxAmmo;
}

void AHW1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind Fire action (requires DefaultInput.ini mapping or manual mapping in Project Settings)
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHW1Character::Fire);

    // Optional basic movement bindings (if you want)
    PlayerInputComponent->BindAxis("MoveForward", this, &ACharacter::AddMovementInput);
    PlayerInputComponent->BindAxis("MoveRight", this, &ACharacter::AddMovementInput);
}

void AHW1Character::Fire()
{
    if (CurrentAmmo <= 0) return;

    if (ProjectileClass)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            FTransform MuzzleTransform = FP_Muzzle ? FP_Muzzle->GetComponentTransform() : GetActorTransform();
            FActorSpawnParameters Params;
            Params.Instigator = this;
            Params.Owner = this;

            World->SpawnActor<AHW1Projectile>(ProjectileClass, MuzzleTransform.GetLocation(), MuzzleTransform.GetRotation().Rotator(), Params);
            CurrentAmmo = FMath::Max(0, CurrentAmmo - 1);
        }
    }
}

void AHW1Character::AddAmmo(int32 Amount)
{
    CurrentAmmo = FMath::Clamp(CurrentAmmo + Amount, 0, MaxAmmo);
}

float AHW1Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (ActualDamage <= 0.f) return 0.f;

    CurrentHealth -= ActualDamage;
    if (CurrentHealth <= 0.f)
    {
        Die();
    }
    return ActualDamage;
}

void AHW1Character::Die()
{
    AController* C = GetController();
    if (C) C->UnPossess();

    SetActorEnableCollision(false);
}
'@

$charCpp | Out-File -FilePath (Join-Path $privateChars "HW1Character.cpp") -Encoding UTF8 -Force

# HW1Projectile.h
$projH = @'
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HW1Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class HW1AGAIN_API AHW1Projectile : public AActor
{
    GENERATED_BODY()

public:
    AHW1Projectile();

protected:
    virtual void BeginPlay() override;

public:
    // Components
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionComp;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float Damage;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
'@

$projH | Out-File -FilePath (Join-Path $publicWeap "HW1Projectile.h") -Encoding UTF8 -Force

# HW1Projectile.cpp
$projCpp = @'
#include "HW1Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AHW1Projectile::AHW1Projectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(8.0f);
    CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
    RootComponent = CollisionComp;

    CollisionComp->OnComponentHit.AddDynamic(this, &AHW1Projectile::OnHit);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    InitialLifeSpan = 3.0f;

    Damage = 20.f;
}

void AHW1Projectile::BeginPlay()
{
    Super::BeginPlay();
}

void AHW1Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this && OtherComp)
    {
        UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigatorController(), this, nullptr);
    }
    Destroy();
}
'@

$projCpp | Out-File -FilePath (Join-Path $privateWeap "HW1Projectile.cpp") -Encoding UTF8 -Force

# DefaultInput.ini (create mapping Fire -> LeftMouseButton)
$inputIni = @'
[/Script/Engine.InputSettings]
+ActionMappings=(ActionName="Fire",Key=LeftMouseButton,bShift=False,bCtrl=False,bAlt=False,bCmd=False)
'@

$inputIni | Out-File -FilePath (Join-Path $configDir "DefaultInput.ini") -Encoding UTF8 -Force

Write-Host "Done: Created Character + Projectile files and DefaultInput.ini in $root"
