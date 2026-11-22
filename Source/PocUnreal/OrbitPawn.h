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

// Glowing cube in the middle of a starry sky, with a camera that orbits around it (yaw only) using the mouse.
// Starts on a menu (Start / Quit). During the simulation:
// left click = new color, arrows = move the orbit center, Escape or Space = back to the menu.
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

	// Soft fill light that follows the camera: avoids completely black faces.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDirectionalLightComponent> FillLight;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CubeMaterial;

	// Current cube hue, in degrees.
	float CubeHue = 0.f;

	// Number of clicks received (debug).
	int32 ClickCount = 0;

	UPROPERTY()
	TObjectPtr<UStaticMesh> StarMesh;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> StarMaterial;

	// Pivot spinning at the center: the small cubes are attached to it.
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

	// Orbit speed, in degrees per second.
	UPROPERTY(EditAnywhere, Category = "Satellites")
	float SatelliteSpeed = 30.f;

	// Orbit center offset applied on each arrow key press.
	UPROPERTY(EditAnywhere, Category = "Satellites")
	float OrbitShiftStep = 15.f;

	// Duration of a full color gradient cycle, in seconds.
	UPROPERTY(EditAnywhere, Category = "Satellites")
	float ColorCycleSeconds = 8.f;

	// Degrees of rotation per pixel of cursor movement.
	UPROPERTY(EditAnywhere, Category = "Orbit")
	float Sensitivity = 0.3f;

	// Last cursor position in the window (to compute the movement).
	FVector2D LastMousePos;
	bool bHasLastMousePos = false;

	// Menu shown over the scene, and its container in the window.
	TSharedPtr<SWidget> MenuWidget;
	TSharedPtr<SWidget> MenuContainer;

	// True during the simulation, false while the menu is shown.
	bool bSimulating = false;

	UPROPERTY(EditAnywhere, Category = "Stars")
	int32 StarCount = 1500;
};
