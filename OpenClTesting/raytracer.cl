struct TriangleIndices{
	int a, b, c;//Indices relative to its objects first vertex
}

struct Triangle{
	float3 a, b, c;
}

struct AABB{
	float3 min;
	float3 max;
}

struct Object{
	AABB boundingBox;
	int startTriangle;
	int startVertex;
	int numTriangles;
	int numVertices;
}

Object objects[NUM_OBJECTS];
TriangleIndices triangles[NUM_TRIANGLES];
float3 vertices[NUM_VERTICES];


float3* getVertices(float3* allVertices, Object object){
	return allVertices[object.startVertex]&;
}

TriangleIndices* getTrianglesIndices(TriangleIndices allTriangles, Object object){
	return allTriangles[object.startTriangle]&;
}

Triangle getTriangle(float* vertices, TriangleIndices triangleIndices){
	Triangle triangle;
	triangle.a = vertices[triangleIndices.a];
	triangle.b = vertices[triangleIndices.b];
	triangle.c = vertices[triangleIndices.c];
	return triangle;
}

Triangle getTriangle(TriangleIndices* trianglesIndices, float* vertices, int index){
	TriangleIndices triangleIndices = trianglesIndices[index];
	return getTriangle(vertices, triangleIndices);
}

void processTriangles(Object object){
	TriangleIndices currentTrianglesIndices = getTrianglesIndices(triangles ,object);
	float3 currentVertices = getVertices(vertices, object);
	
	for(int i = 0; i < object.numTriangles; i++){
		Triangle triangle = getTriangle(currentTrianglesIndices, currentVertices, i);
		
		//Do stuff
	}
	
}