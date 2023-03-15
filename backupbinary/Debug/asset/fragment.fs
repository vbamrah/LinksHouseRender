    #version 330
	
	in vec3 color;
	in vec2 Tcoord;		

	uniform sampler2D brick;
    void main() 
    {
		vec3 outputColor = color;
		if(color.x < 0.1f && color.y < 0.1f && color.z < 0.1f)//this part is used to replace dark vertices with lighter color  
		{
			outputColor = vec3(0.15f);	
		}

		int i = 0;
		gl_FragDepth = 	gl_FragCoord.z;	
        gl_FragColor = vec4(color, 1.0f);
    }