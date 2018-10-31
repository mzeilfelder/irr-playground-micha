uniform vec3  CameraPos;
uniform float Reflectivity;

varying vec3 cubeTC;

void main(void)
{
	gl_Position = ftransform();
	
    // create ray from camera position to the vertex, in world space
    vec3 V = vec3( gl_Vertex ) - CameraPos;
	
    // compute the reflection vector, and assign it to texcoord 0
    gl_TexCoord[0] = vec4( reflect( normalize(V), normalize(gl_Normal) ), 1.0 );

	// just use the normal for the reflection vector
    //gl_TexCoord[0] = normalize(gl_Normal);
}

