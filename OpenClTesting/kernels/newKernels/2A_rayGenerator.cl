

void summarizeRays(global Ray* results, volatile global atomic_int* globalResultCount, Ray result, bool hasResult, int* indexOut);//, volatile local int* groupResultCount);

void kernel rayGenerator(
	global const Hit* hits,
	volatile global atomic_int* rayIndex,
	global Ray* raysOut,
	global RayTree* rayTrees
){
	RayTree rayTree;
	Hit hit = hits[gid];
	rayTree.color = hit.vertex.color;
	rayTree.reflectFactor = hit.vertex.reflectFactor;
	rayTree.refractFactor = hit.vertex.refractFactor;


	bool hasReflection = rayTree.reflectFactor > 0;
	bool hasRefraction = rayTree.refractFactor > 0;
	int reflectionIndex = -1;
	int refractionIndex = -1;
	
	
	
	Ray reflection = reflect(hit);
	Ray refraction = refract(hit);
	
	/*
	volatile local int groupResultCount;
	if(get_local_id(0)==0){																			// First worker will initialize groupResultCount to 0
        //atomic_init(&groupResultCount, 0);
		groupResultCount = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);*/
	summarizeRays(raysOut, rayIndex, reflection, hasReflection, &reflectionIndex);//, &groupResultCount);

	
	/*if(get_local_id(0)==0){																			// First worker will initialize groupResultCount to 0
        //atomic_init(&groupResultCount, 0);
		groupResultCount = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);*/
	summarizeRays(raysOut, rayIndex, refraction, hasRefraction, &refractionIndex);//, &groupResultCount);
	
	rayTree.reflectIndex = reflectionIndex;
	rayTree.refractIndex = refractionIndex;
	
	rayTrees[gid] = rayTree;
}



void summarizeRays(global Ray* results, volatile global atomic_int* globalResultCount, Ray result, bool hasResult, int* indexOut){//, volatile local int* groupResultCount){
	
	int groupIndex;
	int privateIndex;
	
	
	/*if(hasResult){																					// Everyone with a result will reserve themselves an index
		privateIndex = atomic_fetch_add(groupResultCount, 1);
		//privateIndex = atomic_fetch_add_explicit(groupResultCount, 1,
		//					memory_order_relaxed, memory_scope_work_group);
	}
	barrier(CLK_LOCAL_MEM_FENCE);																	//Wait for everyone to reserve themselves a place before next step
    
	
    if(get_local_id(0)==(get_local_size(0)-1)){														// Last worker will commit groupResultCount to globalResultCount
        groupIndex = atomic_fetch_add(globalResultCount, atomic_load(groupResultCount));							//and fetch the group index, thus reserving a section of indices
		//groupIndex = atomic_fetch_add_explicit(globalResultCount, 1,								//for the group
		//					memory_order_relaxed, memory_scope_device);
    }
	barrier(CLK_LOCAL_MEM_FENCE);																	// Make sure everyone in each group waits until the group's 
																									//groupIndex has been recieved
    
	if(hasResult){
		int index = groupIndex + privateIndex;
		*indexOut = index;
		results[index] = result;
	}
	*/
	if(hasResult){
		int index = atomic_fetch_add(globalResultCount, 1);
		*indexOut = index;
		results[index] = result;
	}
	
}

#if 0
void summarizeHitsNew(global Hit* results, volatile global int* globalResultCount, Hit result, bool hasResult){
	int groupIndex;
	int privateIndex;
	
	bool someInGroupHasResult = work_group_any(hasResult);
	
	if(someInGroupHasResult){
		bool allInGroupHasResult = work_group_all(hasResult);
		privateIndex = allInGroupHasResult ?
			get_local_id(0) :
			work_group_scan_exclusive_add(hasResult ? 1 : 0);
	}
	
	barrier(CLK_GLOBAL_MEM_FENCE);
	
	if(someInGroupHasResult){
		if(get_local_id(0) == get_local_size(0) - 1){
			int groupResultCount = privateIndex + (hasResult ? 1 : 0);
			groupIndex = atom_add/*atomic_fetch_add*/(globalResultCount, groupResultCount);
		}
	}
	
	if(hasResult){
		int index = groupIndex + privateIndex;
		results[index] = result;
	}
}
#endif