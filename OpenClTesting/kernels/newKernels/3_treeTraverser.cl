






void kernel treeTraverser(
	global RayTree* rayTrees,
	global read_only RayTree* childRayTrees
){
	int reflectIndex = rayTrees[gid].reflectIndex;
	int refractIndex = rayTrees[gid].refractIndex;
	
	float4 surfaceColor = rayTrees[gid].color;
	
	if(reflectIndex != -1){
		rayTrees[gid].color += surfaceColor * rayTrees[gid].reflectFactor * childRayTrees[reflectIndex].color;
	}
	
	if(refractIndex != -1){
		rayTrees[gid].color += surfaceColor * rayTrees[gid].refractFactor * childRayTrees[refractIndex].color;
		
	}
	/*
	printf("gid: %d\nreflectIndex: %d\nrefractIndex: %d\n\n",
		get_global_id(0),
		reflectIndex,
		refractIndex
	);*/
}
