
#include "stdafx.h"

#include "intersection.hpp"

#include "Containers.hpp"
#include "Utils.hpp"
#include <algorithm>
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

const float EPSILON = glm::epsilon<float>();

inline float maxComponent(float3 vector) {
	return std::max(
		std::max(vector.x, vector.y),
		vector.z
	);
}

inline float minComponent(float3 vector) {
	return std::min(
		std::min(vector.x, vector.y),
		vector.z
	);
}


// http://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
bool intersectsBox(const Ray ray, const AABB boundingBox, float* tmin, float* tmax) {
	float3 minCornerDists = (boundingBox.min - ray.position)*ray.inverseDirection;
	float3 maxCornerDists = (boundingBox.max - ray.position)*ray.inverseDirection;

	*tmin = maxComponent(glm::min(minCornerDists, maxCornerDists));
	*tmax = minComponent(glm::max(minCornerDists, maxCornerDists));

	return *tmin < *tmax &&	//Is Hit
		*tmax > 0;			//Is infront of ray
}

//https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool intersectsTriangle(const Ray ray, const Triangle tri, float* dist, float2* UV) {
	float3 edge1, edge2; //Edge1, Edge2
	float3 P, Q, T;
	float determinant, inv_determinant;
	float u, v;//Triangle uv coordinates
	float tempDist;

	//Find vectors for two edges sharing V1
	edge1 = tri.b.position - tri.a.position;
	edge2 = tri.c.position - tri.a.position;

	//Begin calculating determinant - also used to calculate u parameter 
	P = cross(ray.direction, edge2);
	//if determinant is near zero, ray lies in plane of triangle 
	determinant = dot(edge1, P);
	//NOT CULLING 
	if (determinant > -EPSILON && determinant < EPSILON)
		return false;
	inv_determinant = 1.f / determinant;

	//calculate distance from V1 to ray origin
	T = ray.position - tri.a.position;
	//Calculate u parameter and test bound
	u = dot(T, P) * inv_determinant;
	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f)
		return false;

	//Prepare to test v parameter 
	Q = cross(T, edge1); //Calculate V parameter and test bound
	v = dot(ray.direction, Q) * inv_determinant; //The intersection lies outside of the triangle
	if (v < 0.f || u + v > 1.f)
		return false;
	tempDist = dot(edge2, Q) * inv_determinant;
	if (tempDist > EPSILON)
	{ //ray intersection
		*dist = tempDist;
		UV->x = u;
		UV->y = v;
		return true;
	} // No hit, no win
	return false;
}