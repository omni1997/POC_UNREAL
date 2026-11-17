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
// Démarre sur un menu (Start / Quitter). Pendant la simulation :
// clic gauche = nouvelle couleur, flèches = déplace le centre des orbites, Échap ou Espace = retour au menu.
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

	void ShowMenu();
	void StartSimulation();
	void QuitGame();
	void ApplyGameInput();
	void SetSimulationVisible(bool bVisible);

	void OnClick();
	void OnBackToMenu();
	void ShiftOrbit(float Right, float Up);
	void ShiftOrbitLeft() { ShiftOrbit(-1.f, 0.f); }
	void ShiftOrbitRight() { ShiftOrbit(1.f, 0.f); }
	void ShiftOrbitUp() { ShiftOrbit(0.f, 1.f); }
	void ShiftOrbitDown() { ShiftOrbit(0.f, -1.f); }
	void LogClick();
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

	// Teinte actuelle du cube, en degrés.
	float CubeHue = 0.f;

	// Nombre de clics reçus (debug).
	int32 ClickCount = 0;

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

	// Déplacement du centre des orbites à chaque appui sur une flèche.
	UPROPERTY(EditAnywhere, Category = "Satellites")
	float OrbitShiftStep = 15.f;

	// Durée d'un tour complet du dégradé de couleurs, en secondes.
	UPROPERTY(EditAnywhere, Category = "Satellites")
	float ColorCycleSeconds = 8.f;

	// Degrés de rotation par pixel de déplacement du curseur.
	UPROPERTY(EditAnywhere, Category = "Orbit")
	float Sensitivity = 0.3f;

	// Dernière position du curseur dans la fenêtre (pour calculer le déplacement).
	FVector2D LastMousePos;
	bool bHasLastMousePos = false;

	// Menu affiché par-dessus la scène, et son conteneur dans la fenêtre.
	TSharedPtr<SWidget> MenuWidget;
	TSharedPtr<SWidget> MenuContainer;

	// Vrai pendant la simulation, faux quand le menu est affiché.
	bool bSimulating = false;

	UPROPERTY(EditAnywhere, Category = "Etoiles")
	int32 StarCount = 1500;
};
