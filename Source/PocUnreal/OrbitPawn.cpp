#include "OrbitPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	FLinearColor RandomBrightColor()
	{
		return FLinearColor::MakeFromHSV8(FMath::RandRange(0, 255), 255, 255);
	}
}

AOrbitPawn::AOrbitPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CubeMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	// Matériau non éclairé et opaque avec un paramètre "Color".
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> UnlitMat(TEXT("/Engine/EngineDebugMaterials/LevelColorationUnlitMaterial.LevelColorationUnlitMaterial"));
	StarMesh = SphereMesh.Object;
	StarMaterial = UnlitMat.Object;

	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	RootComponent = Cube;
	Cube->SetStaticMesh(CubeMesh.Object);
	Cube->SetMaterial(0, CubeMat.Object);

	SatellitePivot = CreateDefaultSubobject<USceneComponent>(TEXT("SatellitePivot"));
	SatellitePivot->SetupAttachment(RootComponent);
	// Orbite légèrement inclinée pour donner du relief.
	SatellitePivot->SetRelativeRotation(FRotator(0.f, 0.f, 15.f));

	Arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	Arm->SetupAttachment(RootComponent);
	Arm->TargetArmLength = 500.f;
	Arm->SetRelativeRotation(FRotator(-20.f, 35.f, 0.f));
	Arm->bDoCollisionTest = false;
	Arm->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Arm, USpringArmComponent::SocketName);

	FPostProcessSettings& PP = Camera->PostProcessSettings;
	// Exposition fixe : sinon l'exposition auto éclaircit le fond sombre.
	PP.bOverride_AutoExposureMethod = true;
	PP.AutoExposureMethod = AEM_Manual;
	PP.bOverride_AutoExposureApplyPhysicalCameraExposure = true;
	PP.AutoExposureApplyPhysicalCameraExposure = false;
	PP.bOverride_AutoExposureBias = true;
	PP.AutoExposureBias = 0.f;
	// Bloom : fait rayonner le cube.
	PP.bOverride_BloomIntensity = true;
	PP.BloomIntensity = 4.f;
	// Pas de flou de mouvement (traînées autour du cube quand la caméra tourne vite).
	PP.bOverride_MotionBlurAmount = true;
	PP.MotionBlurAmount = 0.f;

	Light = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Light"));
	Light->SetupAttachment(RootComponent);
	Light->SetUsingAbsoluteRotation(true);
	Light->SetWorldRotation(FRotator(-45.f, 30.f, 0.f));
	Light->SetIntensity(4.f);
}

void AOrbitPawn::BeginPlay()
{
	Super::BeginPlay();

	CubeMaterial = Cube->CreateDynamicMaterialInstance(0);
	SetCubeColor(RandomBrightColor());

	CreateStars();
	CreateSatellites();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Souris libre : curseur visible, pas de capture ni de blocage dans la fenêtre.
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
	if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
	{
		Viewport->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);
		Viewport->SetMouseLockMode(EMouseLockMode::DoNotLock);
	}
}

void AOrbitPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Deux clics rapprochés arrivent comme "Pressed" puis "DoubleClick" : on écoute les deux
	// pour que chaque clic change la couleur.
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AOrbitPawn::OnClick);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_DoubleClick, this, &AOrbitPawn::OnClick);
}

void AOrbitPawn::OnClick()
{
	SetCubeColor(RandomBrightColor());
}

void AOrbitPawn::SetCubeColor(const FLinearColor& Color)
{
	if (CubeMaterial)
	{
		CubeMaterial->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void AOrbitPawn::CreateStars()
{
	if (!StarMesh || !StarMaterial)
	{
		return;
	}

	// Une poignée de teintes : blanc, bleuté, jaune pâle, rosé. Valeurs > 1 pour que le bloom les fasse briller.
	const FLinearColor Tints[] = {
		FLinearColor(1.f, 1.f, 1.f),
		FLinearColor(0.6f, 0.75f, 1.f),
		FLinearColor(1.f, 0.85f, 0.6f),
		FLinearColor(1.f, 0.6f, 0.7f),
	};
	TArray<UMaterialInstanceDynamic*> Materials;
	for (const FLinearColor& Tint : Tints)
	{
		UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(StarMaterial, this);
		Mat->SetVectorParameterValue(TEXT("Color"), Tint * 6.f);
		Materials.Add(Mat);
	}

	FRandomStream Rand(1234);
	for (int32 i = 0; i < StarCount; ++i)
	{
		UStaticMeshComponent* Star = NewObject<UStaticMeshComponent>(this);
		Star->SetStaticMesh(StarMesh);
		Star->SetMaterial(0, Materials[Rand.RandHelper(Materials.Num())]);
		Star->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Star->SetCastShadow(false);

		// Étoiles réparties sur une coquille lointaine ; beaucoup de petites, quelques grosses.
		const FVector Location = Rand.GetUnitVector() * Rand.FRandRange(20000.f, 40000.f);
		const float Size = FMath::Lerp(0.5f, 2.2f, FMath::Pow(Rand.FRand(), 6.f));
		Star->SetWorldLocationAndRotation(Location, FRotator::ZeroRotator);
		Star->SetWorldScale3D(FVector(Size));
		Star->RegisterComponent();
	}
}

void AOrbitPawn::CreateSatellites()
{
	UStaticMesh* CubeMesh = Cube->GetStaticMesh();
	UMaterialInterface* CubeMat = Cube->GetMaterial(0);
	// Cube->GetMaterial renvoie l'instance dynamique : on repart du matériau d'origine.
	if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(CubeMat))
	{
		CubeMat = MID->Parent;
	}

	for (int32 i = 0; i < SatelliteCount; ++i)
	{
		UStaticMeshComponent* Satellite = NewObject<UStaticMeshComponent>(this);
		Satellite->SetStaticMesh(CubeMesh);
		Satellite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Satellite->SetupAttachment(SatellitePivot);

		const float Angle = 2.f * PI * i / SatelliteCount;
		Satellite->SetRelativeLocation(FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * SatelliteRadius);
		Satellite->SetRelativeScale3D(FVector(0.25f));
		Satellite->RegisterComponent();

		Satellites.Add(Satellite);
		SatelliteMaterials.Add(UMaterialInstanceDynamic::Create(CubeMat, this));
		Satellite->SetMaterial(0, SatelliteMaterials.Last());
	}
	UpdateSatellites(0.f);
}

void AOrbitPawn::UpdateSatellites(float DeltaSeconds)
{
	SatellitePivot->AddLocalRotation(FRotator(0.f, SatelliteSpeed * DeltaSeconds, 0.f));

	const float Time = GetWorld()->GetTimeSeconds();
	for (int32 i = 0; i < Satellites.Num(); ++i)
	{
		// Chaque petit cube tourne aussi sur lui-même.
		Satellites[i]->AddLocalRotation(FRotator(40.f, 60.f, 0.f) * DeltaSeconds);

		// Teinte qui défile avec le temps, décalée pour chaque cube.
		const float Hue = FMath::Fmod(Time / ColorCycleSeconds + float(i) / Satellites.Num(), 1.f) * 360.f;
		SatelliteMaterials[i]->SetVectorParameterValue(TEXT("Color"), FLinearColor(Hue, 1.f, 1.f).HSVToLinearRGB());
	}
}

void AOrbitPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateSatellites(DeltaSeconds);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Rotation selon le déplacement horizontal du curseur dans la fenêtre.
		FVector2D MousePos;
		if (PC->GetMousePosition(MousePos.X, MousePos.Y))
		{
			if (bHasLastMousePos)
			{
				const float DeltaX = MousePos.X - LastMousePos.X;
				Arm->AddRelativeRotation(FRotator(0.f, DeltaX * Sensitivity, 0.f));
			}
			LastMousePos = MousePos;
			bHasLastMousePos = true;
		}
		else
		{
			// Curseur hors de la fenêtre : on repart de zéro à son retour.
			bHasLastMousePos = false;
		}
	}
}
