#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OrbitPawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UDirectionalLightComponent;

// Cube au centre, caméra qui tourne autour (yaw uniquement) avec la souris.
UCLASS()
class POCUNREAL_API AOrbitPawn : public APawn
{
	GENERATED_BODY()

public:
	AOrbitPawn();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Cube;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> Arm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDirectionalLightComponent> Light;

	UPROPERTY(EditAnywhere, Category = "Orbit")
	float Sensitivity = 1.5f;
};
