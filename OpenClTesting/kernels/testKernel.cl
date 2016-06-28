typedef struct {
	float3 position;
	float3 direction;
	float3 inverseDirection;//inverseDirection = 1.0f / direction;
}Ray;

typedef struct {
	float3 a, b, c;
} Triangle;

typedef struct {
	int a, b, c;
} TriangleIndices;

typedef struct {
	float3 min;
	float3 max;
} AABB;

typedef struct {
	AABB boundingBox;
	int startTriangle;
	int startVertex;
	int numTriangles;
	int numVertices;
} Object;

void kernel computeKernel(
	global const Object* objects,
	global const TriangleIndices* triangles,
	global const float3* vertices,
	__write_only image2d_t output
){
	int2 pos = (int2)(get_global_id(0),get_global_id(1));
	
	
	//float4 color = (float4)(1, 0, 0, 1);
	float4 color = (float4)(pos.x / 1024.0f, pos.y / 768.0f, 0, 1);
	
	//float4 color = (float4)(pos.x / get_global_size(0), pos.y / get_global_size(1), 0.0f, 1.0f);
	write_imagef(output, pos, color.xyzw);
}
