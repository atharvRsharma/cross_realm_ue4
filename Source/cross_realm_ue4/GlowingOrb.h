#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GlowingOrb.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class UMaterialInstanceDynamic;
class UPrimitiveComponent; 

UCLASS()
class CROSS_REALM_UE4_API AGlowingOrb : public AActor
{
	GENERATED_BODY()

public:
	AGlowingOrb();

	virtual void Tick(float DeltaTime) override;

	void InitializeState(FVector Position, FVector Velocity, float Energy);

protected:
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* OrbMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;


	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY()
	float CurrentEnergy;

	UPROPERTY()
	bool bIsPlayerInteracting;

	UPROPERTY()
	bool bGravityEnabled;

	UPROPERTY()
	FVector InitialPosition;


	UFUNCTION()
	void OnInteractPressed();

	UFUNCTION()
	void OnInteractReleased();

	UFUNCTION()
	void OnToggleGravity();

	UFUNCTION()
	void OnResetPosition();

	UFUNCTION()
	void OnOrbHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};