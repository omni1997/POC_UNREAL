#include "OrbitPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

AOrbitPawn::AOrbitPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	RootComponent = Cube;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Cube->SetStaticMesh(CubeMesh.Object);
	}

	Arm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	Arm->SetupAttachment(RootComponent);
	Arm->TargetArmLength = 500.f;
	Arm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	Arm->bDoCollisionTest = false;
	Arm->bUsePawnControlRotation = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Arm, USpringArmComponent::SocketName);

	Light = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Light"));
	Light->SetupAttachment(RootComponent);
	Light->SetUsingAbsoluteRotation(true);
	Light->SetWorldRotation(FRotator(-45.f, 30.f, 0.f));
}

void AOrbitPawn::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

void AOrbitPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		float DeltaX = 0.f;
		float DeltaY = 0.f;
		PC->GetInputMouseDelta(DeltaX, DeltaY);
		Arm->AddRelativeRotation(FRotator(0.f, DeltaX * Sensitivity, 0.f));
	}
}
