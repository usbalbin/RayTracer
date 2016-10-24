#include "kernels/math.h"

#define xIndex get_global_id(0)
#define yIndex get_global_id(1)
#define width get_global_size(0)
#define height get_global_size(1)

Ray genPerspectiveRay(float16 matrix);

void kernel perspectiveRayGenerator(float16 matrix, Ray* rays) {
	rays[yIndex * width + xIndex] = genPerspectiveRay(matrix);
}

Ray genPerspectiveRay(float16 matrix) {
	Ray ray;
	
	
	ray.position = mulMatVec(matrix, (float4)(0.0f, 0.0f, 7.0f, 1.0f)).xyz;

	
	
	float sideToSide = (float)xIndex / width;
	float topToBottom = (float)yIndex / height;

	float3 tl = (float3)(-1.0f, +1.0f, -1.0f); float3 tr = (float3)(+1.0f, +1.0f, -1.0f);
	float3 bl = (float3)(-1.0f, -1.0f, -1.0f); float3 br = (float3)(+1.0f, -1.0f, -1.0f);

	ray.direction = normalize(
		mix(
			mix(tl, tr, sideToSide),
			mix(bl, br, sideToSide),
			topToBottom
		)
	);

	ray.direction = mulMatVec(matrix, (float4)(ray.direction, 0.0f)).xyz;
	
	ray.inverseDirection = 1.0f / ray.direction;
	return ray;
}