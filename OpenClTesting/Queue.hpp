template<typename T>
class Stack{
	public:
		void push(T element);
		T pop();
		int size();
	private:
		const int SIZE = 10;
		int stackPtr = -1;
		T elements[SIZE];
}

template<typename T>
void Stack::push(T element){
	elements[stackPtr++] = element;
}

template<typename T>
T Stack::pop(){
	return elements[stackPtr--];
}

template<typename T>
void Stack::size(){
	return stackPtr+1;
}