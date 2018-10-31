uniform samplerCube cubeTex;

void main( void )
{
//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	gl_FragColor = textureCube( cubeTex, vec3(gl_TexCoord[0]) );
}

