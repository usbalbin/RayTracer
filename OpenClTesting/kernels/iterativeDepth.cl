#include "kernels/rayTracer.h"
#include "kernels/math.h"

#define MAX_DEAPTH 3
//http://www.geeksforgeeks.org/inorder-tree-traversal-without-recursion/


#define xIndex get_global_id(0)
#define yIndex get_global_id(1)
#define width get_global_size(0)
#define height get_global_size(1)


typedef struct {
	Ray ray;
	Vertex pointOfIntersection;
	float reflect;
} StackFrame;

typedef struct {
	int stackPtr;
	StackFrame data[MAX_DEAPTH - 1];
} Stack;

inline StackFrame pop(Stack* stack){
	return (*stack).data[(*stack).stackPtr--];
}

inline void push(StackFrame frame, Stack* stack){
	(*stack).data[(*stack).stackPtr] = frame;
}

inline int stackLength(Stack* stack){
	return (*stack).stackPtr + 1;
}

inline bool empty(Stack* stack){
	return !stackLength(stack);
}

float4 trace(int objectCount, global const Object* allObjects, global const TriangleIndices* allTriangles, global const Vertex* allVertices, StackFrame* stackFrame, bool* hit);
float4 traverse(int objectCount, global const Object* objects, global const TriangleIndices* triangles, global const Vertex* vertices, float16 matrix);
void getReflectionRay(StackFrame* stackFrame){
	(*stackFrame).ray = reflect((*stackFrame).ray, (*stackFrame).pointOfIntersection);
}

float4 testColor();


void kernel iterative(
	int objectCount,
	float16 matrix,
	global const Object* objects,
	global const TriangleIndices* triangles,
	global const Vertex* vertices,
	__write_only image2d_t output
){
	int2 pos = (int2)(xIndex, height - yIndex);
	float4 color;
	
	//if(pos.x < 15 || pos.x > width - 15 - 1)
		//return;
	
	//float4 color = (float4)(1, 0, 0, 1);
	//float4 color = (float4)(pos.x / 1024.0f, pos.y / 768.0f, 0, 1);
	
	
	color = traverse(objectCount, objects, triangles, vertices, matrix);
	//color = testColor();
	write_imagef(output, pos, color.xyzw);
}





















float4 traverse(int objectCount, global const Object* objects, global const TriangleIndices* triangles, global const Vertex* vertices, float16 matrix){
	StackFrame current;
	current.reflect = 1.0f;
	
	Stack s;
	s.stackPtr = -1;
	Stack* stack = &s;
	bool hit = true;
	current.ray = genPerspectiveRay(matrix);
	float4 result = trace(objectCount, objects, triangles, vertices, &current, &hit);
	if(!hit)
		return result;
	
	while(true){//for(int i = 0; i < 2^(MAX_DEAPTH + 1) - 1; i++)// might me less than 2^(MAX_DEAPTH + 1) - 1 runs !!!
		if(stackLength(stack) < MAX_DEAPTH && hit){
			hit = false;
			push(current, stack);
			float reflect = current.reflect;
			getReflectionRay(&current);
			result += reflect * trace(objectCount, objects, triangles, vertices, &current, &hit);
		}else{
			//If no hit then pop
			
			if(empty(stack))
				break;
			current = pop(stack);
			
			break;
			//float refract = current.refact;
			//getRefractionRay(&current)
			//result += trace(objectCount, objects, triangles, vertices, &current, &hit);
		}
		
	}
	
	return result;
}

float4 trace(int objectCount, global const Object* allObjects, global const TriangleIndices* allTriangles, global const Vertex* allVertices, StackFrame* stackFrame, bool* hit) {
	float4 result = (float4)(0, 0, 0, 1);
	
	float closestTriangleDist = FLT_MAX;
	Triangle closestTriangle;
	float2 closestUv;

	for (int objectIndex = 0; objectIndex < objectCount; objectIndex++) {
		
		Object object = allObjects[objectIndex];
		global const TriangleIndices* triangles = getTrianglesIndices(allTriangles, object);
		global const Vertex* vertices = getVertices(allVertices, object);
		
		float nearDistacnce, farDistance;
		if (!intersectsBox((*stackFrame).ray, object.boundingBox, &nearDistacnce, &farDistance))
			continue;

		result.x = 1;
		//return (float4)(1.0f, 0.0f, 0.0f, 1.0f);
		for (int triangleIndex = 0; triangleIndex < object.numTriangles; triangleIndex++) {
			Triangle triangle = getTriangle(triangles, vertices, object, triangleIndex);

			float distance;
			float2 uv;
			if (intersectsTriangle((*stackFrame).ray, triangle, &distance, &uv) && distance < closestTriangleDist) {
				closestTriangleDist = distance;
				closestTriangle = triangle;
				closestUv = uv;
			}
		}

	}
	
	if(closestTriangleDist == FLT_MAX){
		*hit = false;
		return result;
	}

	
	Vertex interpolated = interpolateTriangle(closestTriangle, closestUv);
	
	result = interpolated.color;
	
	float3 lightDir = normalize((float3)(-0.9f, 0.5f, 0.2f));
	result *= dot(interpolated.normal, -lightDir) * 0.5f + 0.5f;
	
	*hit = true;
	
	//TODO: fresnel stuff or similar 
	//
	//
	(*stackFrame).reflect *= 0.5f;
	(*stackFrame).pointOfIntersection = interpolated;
	
	
	return result;//(float4)((float)(int)closestTriangleDist);
}