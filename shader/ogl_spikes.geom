#version 150 compatibility

#define DRAW_LINES	// switch to line drawing

//layout (points) in;	// has to fit the data - shader written to be a bit flexible
#ifdef DRAW_LINES
layout (line_strip) out; 
#else
layout (triangle_strip) out;
#endif

in VertexData
{
  vec3 color;
  vec2 texCoords;
  vec4 normal;
} inData[];

out geomData
{
	vec3 color;
	vec2 texCoords;
	flat float distance;	// flat says: no need to interpolate value per pixel later
} outData;

uniform mat4 mProjection;

vec3 getPerpendicularUnitVector(vec3 base)
{
	if ( base.x == 0)
	{
		return normalize(cross(base, vec3(1,0,0)));
	}
	else
	{
		return normalize(cross(base, vec3(0,0,1)));
	}
}

void drawOffsetVertexNormal(int i, float scale)
{
	outData.texCoords = inData[i].texCoords;
	outData.color = inData[i].color;
	outData.color.rgb += inData[i].normal.xyz;

	gl_Position =  mProjection * gl_in[i].gl_Position;
	EmitVertex();
	
	gl_Position =  mProjection * (gl_in[i].gl_Position + normalize(inData[i].normal) * scale);	
	EmitVertex();
	
	EndPrimitive();
}

void drawOffset(int i, vec4 offset)
{
	outData.texCoords = inData[i].texCoords;
	outData.color = inData[i].color;
	outData.color.rgb += inData[i].normal.xyz;
	
	gl_Position =  mProjection * gl_in[i].gl_Position;
	EmitVertex();
	
	gl_Position =  mProjection * (gl_in[i].gl_Position + offset);
	EmitVertex();
	
	EndPrimitive();
}

// Just drawing some triangle
void drawBlade(int i, float len, float radius)
{
	outData.texCoords = inData[i].texCoords;
	outData.color = inData[i].color;
	outData.color.rgb += inData[i].normal.xyz;
	
	// Not calculating tangent space for now, so blade rotation is a bit random for now
	vec4 perp = vec4(getPerpendicularUnitVector(inData[i].normal.xyz), 0);

	gl_Position =  mProjection * (gl_in[i].gl_Position + perp * radius);
	EmitVertex();
	
	gl_Position =  mProjection * (gl_in[i].gl_Position - perp * radius);
	EmitVertex();
	
	gl_Position =  mProjection * (gl_in[i].gl_Position + normalize(inData[i].normal) * len);	
	EmitVertex();
	
	EndPrimitive();
}

// only works with triangles
vec4 getFaceNormal()
{
	return vec4(normalize(cross( normalize(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz), normalize(gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz) )), 0.0);
}

void main()
{
#ifdef DRAW_LINES
	for ( int i=0; i < inData.length(); ++i )
		drawOffsetVertexNormal(i, 10);
		
	if ( inData.length() == 3 )
	{
		vec4 faceNormal = getFaceNormal();
		for ( int i=0; i < inData.length(); ++i )
			drawOffset(i, faceNormal*10);
	}
#else
	for ( int i=0; i < inData.length(); ++i )
		drawBlade(i, 20, 2);
#endif	
}
