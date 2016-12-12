#include "kernels/containers.h"

#define gid get_global_id(0)

void kernel aabb(
	//In
	global const 
	Vertex* vertices,
	
	//In and Out
	global Object* objects
	
){
	
	
	Object mesh = objects[gid];
	
	AABB aabb = mesh.boundingBox;
	aabb.min = aabb.max = vertices[mesh.startVertex].position;
	
	for(int i = 1; i < mesh.numVertices; i++){
		aabb.min = min(aabb.min, vertices[mesh.startVertex + i].position);
		aabb.max = max(aabb.max, vertices[mesh.startVertex + i].position);
	}
	objects[gid].boundingBox = aabb;
	
}