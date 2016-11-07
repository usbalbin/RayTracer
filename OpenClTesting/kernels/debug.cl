
kernel void debug(
	volatile global /*atomic_int*/int* counter
){

	//atomic_fetch_add(counter, 1);
	/*Vertex v;
	int reflectlLoc = (float*)&v.reflectFactor - (float*)&v.color;
	int refractlLoc = (float*)&v.refractFactor - (float*)&v.color;
	printf(
		"reflectlLoc: %d\n"
		"refractlLoc: %d\n",
		reflectlLoc,
		refractlLoc
	);*/

}