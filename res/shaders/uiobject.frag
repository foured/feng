#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform int has_tex;
uniform sampler2D tex;
uniform vec3 color;

void main(){
	if(has_tex == 1){
		vec4 color = texture(tex, TexCoords);
		if(color.a < 0.2) 
			discard;
		FragColor = color;
	}
	else{
		FragColor = vec4(color, 1);
	}
}