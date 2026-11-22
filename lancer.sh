#!/usr/bin/env bash
# Runs the game directly (without opening the editor).
# --build: compiles the C++ before running.
set -e

UE_ROOT="${UE_ROOT:-$HOME/Bureau/UnrealEngine}"
PROJET="$(cd "$(dirname "$0")" && pwd)/POC_UNREAL.uproject"

BUILD=0
ARGS=()
for arg in "$@"; do
	if [ "$arg" = "--build" ]; then
		BUILD=1
	else
		ARGS+=("$arg")
	fi
done

if [ "$BUILD" = 1 ]; then
	"$UE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh" PocUnrealEditor Linux Development -Project="$PROJET" -WaitMutex
fi

"$UE_ROOT/Engine/Binaries/Linux/UnrealEditor" "$PROJET" -game -windowed -ResX=1280 -ResY=720 "${ARGS[@]}"
