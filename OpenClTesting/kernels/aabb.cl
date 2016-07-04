#define gid get_gloabal_id(0)

void kernel computeKernel(
	//In
	global const 
	Vertex* vertices,
	
	//In and Out
	global Object* objects
	
){
	Object mesh = objects[gid];
	
	for(int i = 0; i < object.numVertices; i++){
		mesh.boundingBox.min = min(mesh.boundingBox.min, vertices[mesh.startVertex + i]);
		mesh.boundingBox.max = max(mesh.boundingBox.max, vertices[mesh.startVertex + i]);
	}
}