#pragma once

#include "stdafx.h"
#include "Containers.hpp"


inline float maxComponent(float3 vector);
inline float minComponent(float3 vector);

bool intersectsBox(const Ray ray, const AABB boundingBox, float* tmin, float* tmax);
bool intersectsTriangle(const Ray ray, const Triangle tri, float* dist, float2* UV);