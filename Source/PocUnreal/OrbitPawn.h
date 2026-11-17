#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OrbitPawn.generated.h"

class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UDirectionalLightComponent;
class UMaterialInstanceDynamic;
class UStaticMesh;
class UMaterialInterface;

// Cube lumineux au centre d'un ciel étoilé, caméra qui tourne autour (yaw uniquement) avec la souris.
// Clic gauche : le cube prend une couleur aléatoire.
UCLASS()
class POCUNREAL_API AOrbitPawn : public APawn
{
	GENERATED_BODY()

public:
	AOrbitPawn();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	void SetCubeColor(const FLinearColor& Color);
	void CreateStars();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Cube;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> Arm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDirectionalLightComponent> Light;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CubeMaterial;

	UPROPERTY()
	TObjectPtr<UStaticMesh> StarMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> StarMaterial;

	UPROPERTY(EditAnywhere, Category = "Orbit")
	float Sensitivity = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Etoiles")
	int32 StarCount = 1500;
};
