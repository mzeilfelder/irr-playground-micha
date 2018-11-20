uniform samplerCube cubeTex;

void main( void )
{
//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	vec3 uvw = vec3(gl_TexCoord[0]);
	//gl_FragColor = textureCube( cubeTex, uvw );
	gl_FragColor = textureCube( cubeTex, uvw, 4.0 );
}
