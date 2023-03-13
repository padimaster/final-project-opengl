#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out float Ns;

uniform sampler2D texture_specular1;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoords = aTexCoords;
	
    
    vec3 illuminTex  =  texture(texture_specular1, vec2(.25,0.75)).rgb;
    Ns = (illuminTex.r*16777216 + illuminTex.g*65536 + illuminTex.b*256)*0.01;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}