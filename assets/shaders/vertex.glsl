
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in mat4 instanceMatrix;

out vec3 fragNormal;
out vec3 fragPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
   fragPos = vec3(instanceMatrix * vec4(aPos, 1.0));
   fragNormal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
   gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}
