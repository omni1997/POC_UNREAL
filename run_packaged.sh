#!/usr/bin/env bash
# Builds the game as a standalone executable (no editor), then runs it.
set -e

UE_ROOT="${UE_ROOT:-$HOME/Bureau/UnrealEngine}"
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT="$ROOT_DIR/POC_UNREAL.uproject"
OUTPUT_DIR="$ROOT_DIR/Build/Packaged"

"$UE_ROOT/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun \
	-project="$PROJECT" -platform=Linux -clientconfig=Development \
	-build -cook -iterate -stage -pak -archive -archivedirectory="$OUTPUT_DIR" \
	-noP4 -utf8output

"$OUTPUT_DIR/Linux/PocUnreal.sh" -windowed -ResX=1280 -ResY=720 "$@"
