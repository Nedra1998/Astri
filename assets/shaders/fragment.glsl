in vec3 fragNormal;
in vec3 fragPos;

out vec4 FragColor;

uniform vec3 sunPos;

void main() {
  vec3 lightDir = normalize(sunPos - fragPos);
  float diff = max(dot(fragNormal, lightDir), 0.0);
  vec3 result = diff * vec3(1.0) * vec3(1.0, 0.5, 0.2);
  FragColor = vec4(result, 1.0);
}
