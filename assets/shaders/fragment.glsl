in vec3 fragNormal;
in vec3 fragPos;
in vec4 fragColor;

out vec4 FragColor;

uniform vec3 sunPos;
uniform vec3 ambient;

void main() {
  // Ambient
  vec3 ambientColor = ambient * fragColor.rgb;

  // Diffuse
  vec3 norm = normalize(fragNormal);
  vec3 lightDir = normalize(sunPos - fragPos);
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * fragColor.rgb;

  // // Specular
  // vec3 viewDir = normalize(-fragPos);
  // vec3 reflectDir = reflect(-lightDir, norm);
  // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  // vec3 specular = vec3(0.5) * spec;

  FragColor = vec4(ambientColor + diffuse, fragColor.a);
}
