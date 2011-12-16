#version 120

varying mat4 v_tex_rot;
void main(void)
{
	vec4 l_pos = gl_Vertex;
	vec2 l_dir = normalize(vec2(l_pos.z, l_pos.w));

	v_tex_rot = mat4(l_dir.x, l_dir.y, 0.0, 0.0,
			 -l_dir.y, l_dir.x, 0.0, 0.0,
			 0.0, 0.0, 1.0, 0.0,
			 0.0, 0.0, 0.0, 1.0);

	l_pos.z = 0.0;
	l_pos.w = 1.0;
	gl_FrontColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * l_pos;
}