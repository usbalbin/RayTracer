






void kernel treeTraverser(
	global RayTree* rayTrees,
	global const RayTree* childRayTrees
){
	int reflectIndex = rayTrees[gid].reflectIndex;
	int refractIndex = rayTrees[gid].refractIndex;
	
	if(reflectIndex != -1)
		rayTrees[gid].color += rayTrees[gid].reflectFactor * childRayTrees[reflectIndex];
	
	if(refractIndex != -1)
		rayTrees[gid].color += rayTrees[gid].refractFactor * childRayTrees[refractIndex];
}
