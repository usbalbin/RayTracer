#define gid get_gloabal_id(0)

void kernel computeKernel(
	//In
	global const Object* objectTypes,
	global const Instance* instances,

	//Out
	global Object* objects,
	global Vertex* vertices
){
	AABB boundingBox;
	int startTriangle;
	int startVertex;
	int numTriangles;
	int numVertices;
	
	Instance instance = instances[gid];
	ObjectType objectType = objectTypes[instance.meshType];
	float16 modelMatrix = instance.modelMatrix;
	
	Object mesh;
	mesh.startTriangle = objectType.startTriangle;
	mesh.startVertex = instance.startVertex;
	mesh.numTriangles = objectType.numTriangles;
	mesh.numVertices = instance.numVertices;
	
	objects[gid] = mesh;

	for(int i = 0; i < objectType.numVertices; i++){
		vertices[instance.startVertex + i] = mulMatVert(
			modelMatrix,
			objectTypesVertices[objectType.startVertex + i]
		);
	}
}