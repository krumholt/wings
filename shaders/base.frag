#version 330

in vec2 fragment_uv;
in vec4 fragment_color;

uniform sampler2D texture_0;

out vec4 final_color;

void main()
{
    vec4 albedo = texture(texture_0, fragment_uv);
	albedo *= fragment_color;
	if (albedo.a <= 0.1)
		discard;
	float gamma = 2.2;
	final_color = vec4(pow(albedo.rgb, vec3(1.0/gamma)), albedo.a);
}

