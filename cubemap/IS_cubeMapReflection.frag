uniform samplerCube cubeTex;
uniform float FlipRTT; // 1.0 default.  -1.0 for flipping textures (needed for RTT's which are rendered other way round as other textures)

void main( void )
{
//	gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	vec3 uvw = vec3(gl_TexCoord[0]);
	gl_FragColor = textureCube( cubeTex, uvw );
}
