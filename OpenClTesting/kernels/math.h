#pragma once
#include "kernels/containers.h"

float4 mulMatVec(float16 m, float4 v){
	return (float4)(
		dot(m.s048c, v),
		dot(m.s159d, v),
		dot(m.s26ae, v),
		dot(m.s37bf, v)
	);
}
/*
//TODO check wether m1 and m2 should swap places
float16 mulMat_old(float16 m1, float16 m2){
	return (float16)(
		dot(m1.s0123, m2.s048c), dot(m1.s0123, m2.s159d), dot(m1.s0123, m2.s26ae), dot(m1.s0123, m2.s37bf),
		dot(m1.s4567, m2.s048c), dot(m1.s4567, m2.s159d), dot(m1.s4567, m2.s26ae), dot(m1.s4567, m2.s37bf),
		dot(m1.s89ab, m2.s048c), dot(m1.s89ab, m2.s159d), dot(m1.s89ab, m2.s26ae), dot(m1.s89ab, m2.s37bf),
		dot(m1.scdef, m2.s048c), dot(m1.scdef, m2.s159d), dot(m1.scdef, m2.s26ae), dot(m1.scdef, m2.s37bf)
	);
}
*/

Ray reflect(Hit hit){
	const float epsilon = 1e-6f;
	const Ray rayIn = hit.ray;
	const Vertex poi = hit.vertex;
	
	
	Ray result;
	
	//Move position along normal slightly to prevent collision with reflector
	result.position = poi.position + epsilon * poi.normal;
	result.direction = rayIn.direction - 2.0f * dot(rayIn.direction, poi.normal) * poi.normal;
	result.inverseDirection = 1.0f / result.direction;
	
	return result;
}

/**
 * TODO add support for refraction index
 */
Ray refract(Hit hit){
	const float epsilon = 1e-6f;
	const Ray rayIn = hit.ray;
	const Vertex poi = hit.vertex;
	
	Ray result;
	
	//Move position along normal slightly to prevent collision with refractor
	result.position = poi.position - epsilon * poi.normal;
	result.direction = rayIn.direction;
	result.inverseDirection = rayIn.inverseDirection;
	
	return result;
}