#version 150 compatibility

//#define DRAW_LINES	// switch to line drawing

layout (triangles) in;

#ifdef DRAW_LINES
layout (line_strip) out;	 // can also have: ,max_vertices=value to restrict output
#else
layout (triangle_strip) out;
#endif

in VertexData
{
  vec3 color;
  vec2 texCoords;
  vec4 normal;
} inData[3];

out geomData
{
	vec3 color;
	vec2 texCoords;
	flat float distance;	// flat says: no need to interpolate value per pixel later
} outData;

uniform mat4 mProjection;

#ifdef DRAW_LINES

void drawLine(int i1, int i2)
{
	gl_Position =  mProjection * gl_in[i1].gl_Position;
	outData.texCoords = inData[i1].texCoords;
	outData.color = inData[i1].color;
	EmitVertex();
	
	gl_Position =  mProjection * gl_in[i2].gl_Position;
	outData.texCoords = inData[i2].texCoords;
	outData.color = inData[i2].color;
	EmitVertex();
	
	EndPrimitive();
}

void drawOffset(int i, float offset)
{
	outData.texCoords = inData[i].texCoords;
	outData.color = inData[i].color;

	gl_Position =  mProjection * gl_in[i].gl_Position;
	EmitVertex();
	
	gl_Position =  mProjection * (gl_in[i].gl_Position + inData[i].normal * offset);
	EmitVertex();
	
	EndPrimitive();
}

void main()
{
	drawLine(0, 1);
	drawLine(1, 2);
	drawLine(2, 0);
	
	float s = 30;
	
	drawOffset(0, s);
	drawOffset(1, s);
	drawOffset(2, s);
}

#else	// triangles out

void drawShell(float offset)
{
	outData.distance = offset;

	gl_Position = mProjection * (gl_in[0].gl_Position + inData[0].normal*offset);
	outData.texCoords = inData[0].texCoords;
	outData.color = inData[0].color;
	EmitVertex();
	
	gl_Position = mProjection * (gl_in[1].gl_Position + inData[1].normal*offset);
	outData.texCoords = inData[1].texCoords;
	outData.color = inData[1].color;
	EmitVertex();
	
	gl_Position = mProjection * (gl_in[2].gl_Position + inData[2].normal*offset);
	outData.texCoords = inData[2].texCoords;
	outData.color = inData[2].color;
	EmitVertex();
	
	EndPrimitive();	
}

void main()
{
//	gl_InvocationID;

	// Start by drawing the original triangles
	outData.distance = 0;
	
	gl_Position = mProjection * gl_in[0].gl_Position; 
	outData.texCoords = inData[0].texCoords;
	outData.color = inData[0].color;
	EmitVertex();
	
	gl_Position = mProjection * gl_in[1].gl_Position; 
	outData.texCoords = inData[1].texCoords;
	outData.color = inData[1].color;
	EmitVertex();
	
	gl_Position = mProjection * gl_in[2].gl_Position;
	outData.texCoords = inData[2].texCoords;
	outData.color = inData[2].color;
	EmitVertex();
	
	EndPrimitive();

	// Now draw some shells - same polygons by offset by the normal*some_factor
	//
	// This loop will quickly reach the limit of maximal vertex data a geometry shader is allowed to push out
	// The limit is the amount of vertices * size of the data output
	// So the smaller geomData the more vertices you can push out
	// It's CPU specific (for example 1024 for mine)
	for(int i=0;i<10;++i)
		drawShell(float(i)*9);
}
#endif
