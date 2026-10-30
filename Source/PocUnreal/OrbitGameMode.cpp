#include "OrbitGameMode.h"
#include "OrbitPawn.h"

AOrbitGameMode::AOrbitGameMode()
{
	DefaultPawnClass = AOrbitPawn::StaticClass();
}
