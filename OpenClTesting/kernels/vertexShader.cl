
#include "kernels/containers.h"
#include "kernels/math.h"

#define gid get_global_id(0)

Vertex transformVertex(float16 matrix, Vertex vertex);

void kernel vertexShader(
	//In
	global const Object* objectTypes,
	global const Instance* instances,
	global const Vertex* objectTypesVertices,

	//Out
	global Object* objects,
	global Vertex* vertices
){
	Instance instance = instances[gid];
	Object objectType = objectTypes[instance.meshType];
	float16 modelMatrix = instance.modelMatrix;

	/*if(gid == 0){
		printf("instances[1].startTriangle: %d\n", instances[1].startVertex);
	}*/
	
	Object mesh;
	mesh.startTriangle = objectType.startTriangle;
	mesh.startVertex = instance.startVertex;
	mesh.numTriangles = objectType.numTriangles;
	mesh.numVertices = objectType.numVertices;
	
	objects[gid] = mesh;

	for(int i = 0; i < objectType.numVertices; i++){
		vertices[mesh.startVertex + i] = 
			transformVertex(
				modelMatrix, 
				objectTypesVertices[objectType.startVertex + i]
			);
	}
}

Vertex transformVertex(float16 matrix, Vertex vertex){
	Vertex result = vertex;
	
	result.position = mulMatVec(
		matrix,
		(float4)(vertex.position, 1.0f)
	).xyz;
	
	result.normal = mulMatVec(
		matrix,
		(float4)(vertex.normal, 0.0f)
	).xyz;
	return result;
}