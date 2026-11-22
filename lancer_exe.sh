#!/usr/bin/env bash
# Builds the game as a standalone executable (no editor), then runs it.
set -e

UE_ROOT="${UE_ROOT:-$HOME/Bureau/UnrealEngine}"
DOSSIER="$(cd "$(dirname "$0")" && pwd)"
PROJET="$DOSSIER/POC_UNREAL.uproject"
SORTIE="$DOSSIER/Build/Packaged"

"$UE_ROOT/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun \
	-project="$PROJET" -platform=Linux -clientconfig=Development \
	-build -cook -iterate -stage -pak -archive -archivedirectory="$SORTIE" \
	-noP4 -utf8output

"$SORTIE/Linux/PocUnreal.sh" -windowed -ResX=1280 -ResY=720 "$@"
