#define MAX_DEAPTH 3



typedef struct {
	Ray rayIn;
	Vertex pointOfIntersection;
	
} StackFrame;

typedef struct {
	int stackPtr = -1;
	StackFrame data[MAX_DEAPTH - 1];
} Stack;

inline StackFrame pop(Stack* stack){
	return stack.data[stack->stackPtr--];
}

inline void push(StackFrame frame, Stack* stack){
	stack.data[++stack->stackPtr] = frame;
}

inline int length(Stack* stack){
	return stack->stackPtr + 1;
}

inline bool empty(Stack* stack){
	return !length(stack);
}

float4 traverse(){
	StackFrame current;
	Stack s;
	Stack* stack = &s;
	float4 result = traceBruteForceColor(genPerspectiveRay(), &current);
	
	while(true){//for(int i = 0; i < 2^(MAX_DEAPTH + 1) - 1; i++)
		if(length(stack) < MAX_DEAPTH){
			push(current, stack);
			
			result += traceBruteForceColor(getRefractionRay(current), &current)
		
		}else{
			if(empty(stack))//replace if-statement with: for (int i = 0; i < 2^(MAX_DEAPTH + 1) - 1; i++)
				break;
			
			result += traceBruteForceColor(getRefractionRay(current), &current)
		}
		
	}
	
	return result;
}