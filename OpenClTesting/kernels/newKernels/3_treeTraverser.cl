






void kernel treeTraverser(
	global RayTree* rayTrees,
	global const RayTree* childRayTrees
){
	int reflectIndex = rayTrees[gid].reflectIndex;
	int refractIndex = rayTrees[gid].refractIndex;
	
	if(reflectIndex != -1)
		rayTrees[gid].color += 0.5f/*rayTrees[gid].reflectFactor*/ * childRayTrees[reflectIndex].color;
	
	if(refractIndex != -1)
		rayTrees[gid].color += 0.5f/*rayTrees[gid].refractFactor*/ * childRayTrees[refractIndex].color;
}
