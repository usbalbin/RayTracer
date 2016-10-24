#define xIndex get_global_id(0)
#define yIndex get_global_id(1)
#define width get_global_size(0)
#define height get_global_size(1)


void kernel colorToPixel(
	global const RayTree* rayTrees,
	__write_only image2d_t output
){
	int2 pos = (int2)(xIndex, height - yIndex);
	float4 color = rayTrees[yIndex * width + xIndex].color;
	
	
	
	
	
	write_imagef(output, pos, color.xyzw);
}