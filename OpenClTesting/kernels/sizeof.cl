#include "kernels/containers.h"

kernel void debug(global int* output){
	*output = sizeof(Instance);
}