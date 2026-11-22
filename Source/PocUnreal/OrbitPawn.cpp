#include "OrbitPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SOrbitMenu.h"
#include "Widgets/SWeakWidget.h"
#include "UObject/ConstructorHelpers.h"

AOrbitPawn::AOrbitPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> CubeMat(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	// Unlit opaque material with a "Color" parameter.
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> UnlitMat(TEXT("/Engine/EngineDebugMaterials/LevelColorationUnlitMaterial.LevelColorationUnlitMaterial"));
	StarMesh = SphereMesh.Object;
	StarMaterial = UnlitMat.Object;

	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	RootComponent = Cube;
	Cube->SetStaticMesh(CubeMesh.Object);
	Cube->SetMaterial(0, CubeMat.Object);

	SatellitePivot = CreateDefaultSubobject<USceneComponent>(TEXT("SatellitePivot"));
	SatellitePivot->SetupAttachment(RootComponent);
	// Slightly tilted orbit to add depth.
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
	// Fixed exposure: otherwise auto exposure brightens the dark background.
	PP.bOverride_AutoExposureMethod = true;
	PP.AutoExposureMethod = AEM_Manual;
	PP.bOverride_AutoExposureApplyPhysicalCameraExposure = true;
	PP.AutoExposureApplyPhysicalCameraExposure = false;
	PP.bOverride_AutoExposureBias = true;
	PP.AutoExposureBias = 0.f;
	// Bloom: makes the cube glow.
	PP.bOverride_BloomIntensity = true;
	PP.BloomIntensity = 4.f;
	// No motion blur (trails around the cube when the camera turns fast).
	PP.bOverride_MotionBlurAmount = true;
	PP.MotionBlurAmount = 0.f;

	Light = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Light"));
	Light->SetupAttachment(RootComponent);
	Light->SetUsingAbsoluteRotation(true);
	Light->SetWorldRotation(FRotator(-45.f, 30.f, 0.f));
	Light->SetIntensity(4.f);

	FillLight = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("FillLight"));
	FillLight->SetupAttachment(Camera);
	// Lights along the view direction, slightly offset to keep a gradient between faces.
	FillLight->SetRelativeRotation(FRotator(-15.f, 20.f, 0.f));
	FillLight->SetIntensity(1.2f);
	FillLight->SetCastShadows(false);
	FillLight->SetLightColor(FLinearColor(0.75f, 0.8f, 1.f));
}

void AOrbitPawn::BeginPlay()
{
	Super::BeginPlay();

	CubeMaterial = Cube->CreateDynamicMaterialInstance(0);
	CubeHue = FMath::FRandRange(0.f, 360.f);
	SetCubeColor(FLinearColor(CubeHue, 1.f, 1.f).HSVToLinearRGB());

	CreateStars();
	CreateSatellites();

	ShowMenu();
}

void AOrbitPawn::ShowMenu()
{
	bSimulating = false;
	SetSimulationVisible(false);

	UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
	if (!Viewport)
	{
		return;
	}
	if (!MenuWidget)
	{
		TWeakObjectPtr<AOrbitPawn> WeakThis(this);
		MenuWidget = SNew(SOrbitMenu)
			.OnStart_Lambda([WeakThis]() { if (WeakThis.IsValid()) { WeakThis->StartSimulation(); } })
			.OnQuit_Lambda([WeakThis]() { if (WeakThis.IsValid()) { WeakThis->QuitGame(); } });
		MenuContainer = SNew(SWeakWidget).PossiblyNullContent(MenuWidget);
	}
	Viewport->AddViewportWidgetContent(MenuContainer.ToSharedRef());

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void AOrbitPawn::StartSimulation()
{
	if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
	{
		if (MenuContainer)
		{
			Viewport->RemoveViewportWidgetContent(MenuContainer.ToSharedRef());
		}
	}

	// New simulation: orbits recentered on the cube.
	SatellitePivot->SetRelativeLocation(FVector::ZeroVector);
	SetSimulationVisible(true);
	// Avoids a camera jump: the cursor moved while the menu was shown.
	bHasLastMousePos = false;
	bSimulating = true;
	ApplyGameInput();
}

void AOrbitPawn::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, Cast<APlayerController>(GetController()), EQuitPreference::Quit, false);
}

void AOrbitPawn::SetSimulationVisible(bool bVisible)
{
	// The cube alone (its lights and the camera are attached to it), then the satellites with their pivot.
	Cube->SetHiddenInGame(!bVisible, false);
	SatellitePivot->SetHiddenInGame(!bVisible, true);
}

void AOrbitPawn::ApplyGameInput()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Free mouse: visible cursor, not locked to the window.
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
	if (UGameViewportClient* Viewport = GetWorld()->GetGameViewport())
	{
		// Capture only while the button is held: without capture, the engine does not send single clicks to the game.
		Viewport->SetMouseCaptureMode(EMouseCaptureMode::CaptureDuringMouseDown);
		Viewport->SetMouseLockMode(EMouseLockMode::DoNotLock);
	}
}

void AOrbitPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// A double click also arrives as "Pressed": no need to listen to IE_DoubleClick.
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AOrbitPawn::OnClick);
	PlayerInputComponent->BindKey(EKeys::SpaceBar, IE_Pressed, this, &AOrbitPawn::OnBackToMenu);
	PlayerInputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AOrbitPawn::OnBackToMenu);

	// Arrows: one step per press, and repeated steps while the key is held.
	for (const EInputEvent Event : { IE_Pressed, IE_Repeat })
	{
		PlayerInputComponent->BindKey(EKeys::Left, Event, this, &AOrbitPawn::ShiftOrbitLeft);
		PlayerInputComponent->BindKey(EKeys::Right, Event, this, &AOrbitPawn::ShiftOrbitRight);
		PlayerInputComponent->BindKey(EKeys::Up, Event, this, &AOrbitPawn::ShiftOrbitUp);
		PlayerInputComponent->BindKey(EKeys::Down, Event, this, &AOrbitPawn::ShiftOrbitDown);
	}
}

void AOrbitPawn::OnBackToMenu()
{
	if (bSimulating)
	{
		ShowMenu();
	}
}

void AOrbitPawn::ShiftOrbit(float Right, float Up)
{
	if (!bSimulating)
	{
		return;
	}
	// Move in the screen plane: left/right and up/down as seen by the camera.
	const FVector Offset = Camera->GetRightVector() * Right + Camera->GetUpVector() * Up;
	SatellitePivot->AddWorldOffset(Offset * OrbitShiftStep);
}

void AOrbitPawn::LogClick()
{
	FVector2D MousePos(-1.f, -1.f);
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->GetMousePosition(MousePos.X, MousePos.Y);
	}
	++ClickCount;
	UE_LOG(LogTemp, Warning, TEXT("Clic #%d position x=%.0f y=%.0f"), ClickCount, MousePos.X, MousePos.Y);
}

void AOrbitPawn::OnClick()
{
	if (!bSimulating)
	{
		return;
	}
	LogClick();
	// New hue always at least 60° away from the current one: the change is visible on every click.
	CubeHue = FMath::Fmod(CubeHue + FMath::FRandRange(60.f, 300.f), 360.f);
	SetCubeColor(FLinearColor(CubeHue, 1.f, 1.f).HSVToLinearRGB());
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

	// A handful of tints: white, bluish, pale yellow, pinkish. Values > 1 so that bloom makes them shine.
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

		// Stars spread over a distant shell; many small ones, a few big ones.
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
	// Cube->GetMaterial returns the dynamic instance: start again from the original material.
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
		// Each small cube also spins on itself.
		Satellites[i]->AddLocalRotation(FRotator(40.f, 60.f, 0.f) * DeltaSeconds);

		// Hue that cycles over time, offset for each cube.
		const float Hue = FMath::Fmod(Time / ColorCycleSeconds + float(i) / Satellites.Num(), 1.f) * 360.f;
		SatelliteMaterials[i]->SetVectorParameterValue(TEXT("Color"), FLinearColor(Hue, 1.f, 1.f).HSVToLinearRGB());
	}
}

void AOrbitPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bSimulating)
	{
		return;
	}

	UpdateSatellites(DeltaSeconds);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Rotation from the horizontal cursor movement in the window.
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
			// Cursor outside the window: start over when it comes back.
			bHasLastMousePos = false;
		}
	}
}
