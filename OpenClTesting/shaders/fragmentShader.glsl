#version 330

//Interpolated textureCoordinates from vertex shader
in vec2 UV;

//Color of fragment
out vec4 color;

//Sampler for getting colors from texture
uniform sampler2D textureSampler;

void main(){
	color = texture( textureSampler, UV );
}