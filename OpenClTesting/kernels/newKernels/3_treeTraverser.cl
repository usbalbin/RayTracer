






void kernel treeTraverser(
	global RayTree* rayTrees,
	global const RayTree* childRayTrees
){
	int reflectIndex = rayTrees[get_global_id(0)].reflectIndex;
	int refractIndex = rayTrees[get_global_id(0)].refractIndex;
	
	if(reflectIndex != -1){
		//rayTrees[gid].color += rayTrees[gid].reflectFactor * childRayTrees[reflectIndex].color;
	}
	
	if(refractIndex != -1){
		//rayTrees[gid].color += rayTrees[gid].refractFactor * childRayTrees[refractIndex].color;
		
	}
	
	printf("reflectIndex: %d\n", get_global_id(0));
	printf("reflectIndex: %d\n", reflectIndex);
	printf("refractIndex: %d\n\n", refractIndex);
}
