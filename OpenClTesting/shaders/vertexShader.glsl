#version 330

layout(location = 0) in vec2 position;

// Output data, will be interpolated for each fragment.
out vec2 UV;

void main(){
	gl_Position = vec4(position, 0.0, 1.0);
	UV = position.xy * 0.5 + 0.5;
}