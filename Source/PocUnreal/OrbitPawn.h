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
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void OnClick();
	void SetCubeColor(const FLinearColor& Color);
	void CreateStars();
	void CreateSatellites();
	void UpdateSatellites(float DeltaSeconds);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Cube;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> Arm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDirectionalLightComponent> Light;

	// Lumière d'appoint douce qui suit la caméra : évite les faces complètement noires.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDirectionalLightComponent> FillLight;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CubeMaterial;

	UPROPERTY()
	TObjectPtr<UStaticMesh> StarMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> StarMaterial;

	// Pivot qui tourne au centre : les petits cubes y sont accrochés.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SatellitePivot;

	UPROPERTY()
	TArray<TObjectPtr<UStaticMeshComponent>> Satellites;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> SatelliteMaterials;

	UPROPERTY(EditAnywhere, Category = "Satellites")
	int32 SatelliteCount = 6;

	UPROPERTY(EditAnywhere, Category = "Satellites")
	float SatelliteRadius = 200.f;

	// Vitesse de l'orbite, en degrés par seconde.
	UPROPERTY(EditAnywhere, Category = "Satellites")
	float SatelliteSpeed = 30.f;

	// Durée d'un tour complet du dégradé de couleurs, en secondes.
	UPROPERTY(EditAnywhere, Category = "Satellites")
	float ColorCycleSeconds = 8.f;

	// Degrés de rotation par pixel de déplacement du curseur.
	UPROPERTY(EditAnywhere, Category = "Orbit")
	float Sensitivity = 0.3f;

	// Dernière position du curseur dans la fenêtre (pour calculer le déplacement).
	FVector2D LastMousePos;
	bool bHasLastMousePos = false;

	UPROPERTY(EditAnywhere, Category = "Etoiles")
	int32 StarCount = 1500;
};
