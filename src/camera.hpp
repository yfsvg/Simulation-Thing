#pragma once
#include "raylib.h"

extern Camera2D camera;
extern float cameraSpeed;
extern int panButton;

void initCamera();
void cameraMovement();