#pragma once
#include "raylib.h"

#include "environment.hpp"

void initCamera(Camera2D& camera);
void cameraMovement(Camera2D& camera, const GameMap& map);