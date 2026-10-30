#!/usr/bin/env bash
# Compile puis lance le jeu directement (sans ouvrir l'éditeur).
set -e

UE_ROOT="${UE_ROOT:-$HOME/Bureau/UnrealEngine}"
PROJET="$(cd "$(dirname "$0")" && pwd)/POC_UNREAL.uproject"

"$UE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh" PocUnrealEditor Linux Development -Project="$PROJET" -WaitMutex
"$UE_ROOT/Engine/Binaries/Linux/UnrealEditor" "$PROJET" -game -windowed -ResX=1280 -ResY=720 "$@"
