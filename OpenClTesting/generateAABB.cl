float3* getVertices(Object object){
	return vertices[object.startVertex]&;
}


AABB genAABB(float3* vertices, int vertexCount){
	AABB res;
	res.max = res.min = vertices[0];
	
	for(int i = 1; i < vertexCount; i++){
		res.min = min(vertices[i], res.min);
		res.max = max(vertices[i], res.min);
	}
	
	return res;
}

void kernel generateAABB(Object* objects, float3* vertices){
	int i = global_id(0);
	
	float3* vertices = getVertices(objects[i]);
	int vertexCount = object.numVertices;
	
	object.boundingBox = genAABB(vertices, vertexCount);
}
	