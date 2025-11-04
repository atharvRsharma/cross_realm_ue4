#include "GlowingOrb.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/InputComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h" 

AGlowingOrb::AGlowingOrb()
{
	PrimaryActorTick.bCanEverTick = true;

	OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	RootComponent = OrbMesh;

	OrbMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	OrbMesh->SetSimulatePhysics(false);
	OrbMesh->SetNotifyRigidBodyCollision(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.6f;

	bGravityEnabled = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	InitialPosition = FVector(0.0f, 0.0f, 500.0f);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("Material'/Game/M_GlowingOrb.M_GlowingOrb'"));

	if (MaterialFinder.Succeeded())
	{
		OrbMesh->SetMaterial(0, MaterialFinder.Object);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AGlowingOrb: COULD NOT FIND M_GlowingOrb MATERIAL! Check path in constructor."));
	}
}

void AGlowingOrb::BeginPlay()
{
	Super::BeginPlay();

	EnableInput(GetWorld()->GetFirstPlayerController());

	if (InputComponent)
	{
		InputComponent->BindAction("Interact", IE_Pressed, this, &AGlowingOrb::OnInteractPressed).bConsumeInput = true;
		InputComponent->BindAction("Interact", IE_Released, this, &AGlowingOrb::OnInteractReleased);
		InputComponent->BindAction("ToggleGravity", IE_Pressed, this, &AGlowingOrb::OnToggleGravity).bConsumeInput = true;
		InputComponent->BindAction("ResetPosition", IE_Pressed, this, &AGlowingOrb::OnResetPosition).bConsumeInput = true;
	}

	if (OrbMesh->GetMaterial(0))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(OrbMesh->GetMaterial(0), this);
		OrbMesh->SetMaterial(0, DynamicMaterial);

		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(TEXT("Energy"), CurrentEnergy);
		}
	}

	OrbMesh->OnComponentHit.AddDynamic(this, &AGlowingOrb::OnOrbHit);
}

void AGlowingOrb::InitializeState(FVector Position, FVector Velocity, float Energy, bool bInitialGravityState)
{
	SetActorLocation(Position);
	ProjectileMovement->SetVelocityInLocalSpace(Velocity);
	CurrentEnergy = Energy;

	bGravityEnabled = bInitialGravityState;

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Energy"), CurrentEnergy);
	}
}

void AGlowingOrb::OnInteractPressed() { bIsPlayerInteracting = true; }
void AGlowingOrb::OnInteractReleased() { bIsPlayerInteracting = false; }


void AGlowingOrb::OnToggleGravity()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("TOGGLE GRAVITY PRESSED!"));

	bGravityEnabled = !bGravityEnabled;

	if (!bGravityEnabled)
	{
		ProjectileMovement->SetVelocityInLocalSpace(FVector::ZeroVector);
	}
}


void AGlowingOrb::OnResetPosition()
{
	SetActorLocation(InitialPosition);
	ProjectileMovement->SetVelocityInLocalSpace(FVector::ZeroVector);
	CurrentEnergy = 0.5f;

}

void AGlowingOrb::OnOrbHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	float ImpactVelocity = FMath::Abs(GetVelocity().Z);
	const float RestThreshold = 10.0f;
	if (ImpactVelocity > RestThreshold)
	{
		float EnergyGain = ImpactVelocity * 0.001f;
		CurrentEnergy = FMath::Clamp(CurrentEnergy + EnergyGain, 0.0f, 1.0f);
	}
}


void AGlowingOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsPlayerInteracting)
	{
		CurrentEnergy = FMath::Clamp(CurrentEnergy + (1.0f * DeltaTime), 0.0f, 1.0f);
	}
	else
	{
		CurrentEnergy = FMath::Clamp(CurrentEnergy - (0.1f * DeltaTime), 0.0f, 1.0f);
	}

	if (DynamicMaterial)
	{
		DynamicMaterial->SetScalarParameterValue(TEXT("Energy"), CurrentEnergy);
	}

	if (bGravityEnabled)
	{
		FVector vel = ProjectileMovement->Velocity;
		vel.Z += -980.f * DeltaTime;
		ProjectileMovement->Velocity = vel;
	}
	else
	{
		FVector vel = ProjectileMovement->Velocity;
		if (FMath::Abs(vel.Z) < 1.f)
			vel.Z = 0.f;
		ProjectileMovement->Velocity = vel;
	}
}