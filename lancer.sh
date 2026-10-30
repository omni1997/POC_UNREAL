#!/usr/bin/env bash
# Compile puis lance l'éditeur Unreal avec le projet.
set -e

UE_ROOT="${UE_ROOT:-$HOME/Bureau/UnrealEngine}"
PROJET="$(cd "$(dirname "$0")" && pwd)/POC_UNREAL.uproject"

"$UE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh" PocUnrealEditor Linux Development -Project="$PROJET" -WaitMutex
"$UE_ROOT/Engine/Binaries/Linux/UnrealEditor" "$PROJET" "$@"
