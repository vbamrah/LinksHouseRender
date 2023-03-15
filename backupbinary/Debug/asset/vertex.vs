    #version 330
    layout(location = 0)in vec3 pos;
    layout(location = 1)in vec2 coord;
    
	out vec3 color;
	out vec2 Tcoord;
	

	uniform mat4 model;
	uniform mat4 projection;
	uniform mat4 view;

	void main()
    {
        gl_Position = projection * view * model * vec4(pos, 1);
		color = pos + (vec3(0.2f, 0.2f, 0.2f) * 0.25f * gl_VertexID);
        Tcoord = coord;
    }