#include "Characters/HW1Character.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

// Constructor
AHW1Character::AHW1Character()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a default muzzle location component and attach to root
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GetRootComponent());
	MuzzleLocation->SetRelativeLocation(FVector(100.f, 0.f, 50.f)); // forward + up offset

	Ammo = 30;
}

void AHW1Character::BeginPlay()
{
	Super::BeginPlay();
}

void AHW1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHW1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind movement axes (make sure these exist in Project Settings -> Input)
	PlayerInputComponent->BindAxis("MoveForward", this, &AHW1Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHW1Character::MoveRight);

	// Bind fire action (make sure "Fire" action mapping exists)
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHW1Character::OnFire);

	// Jump (if you want - standard Character)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

void AHW1Character::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AHW1Character::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AHW1Character::OnFire()
{
	if (Ammo <= 0)
	{
		// optionally: play empty sound or UI notice
		UE_LOG(LogTemp, Warning, TEXT("No ammo"));
		return;
	}

	if (ProjectileClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileClass is not set on %s"), *GetName());
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		FVector SpawnLoc = MuzzleLocation ? MuzzleLocation->GetComponentLocation() : GetActorLocation() + GetActorForwardVector() * 100.f;
		FRotator SpawnRot = GetControlRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		AActor* Spawned = World->SpawnActor<AActor>(ProjectileClass, SpawnLoc, SpawnRot, SpawnParams);
		if (Spawned)
		{
			Ammo--;
			// Debug message
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Ammo: %d"), Ammo));
			}
		}
	}
}
