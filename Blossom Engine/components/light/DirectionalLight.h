#pragma once
#include "../Blossom Engine/Math/Vector3.h"
#include "../Blossom Engine/Math/Vector4.h"

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};