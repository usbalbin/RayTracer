






void kernel treeTraverser(
	global RayTree* rayTrees,
	global read_only RayTree* childRayTrees
){
	int reflectIndex = rayTrees[get_global_id(0)].reflectIndex;
	int refractIndex = rayTrees[get_global_id(0)].refractIndex;
	
	if(reflectIndex != -1){
		rayTrees[gid].color += rayTrees[gid].reflectFactor * childRayTrees[reflectIndex].color;
	}
	
	if(refractIndex != -1){
		rayTrees[gid].color += rayTrees[gid].refractFactor * childRayTrees[refractIndex].color;
		
	}
	/*
	printf("gid: %d\nreflectIndex: %d\nrefractIndex: %d\n\n",
		get_global_id(0),
		reflectIndex,
		refractIndex
	);*/
}
