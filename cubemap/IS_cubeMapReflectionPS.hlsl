sampler cubeTex: register(s0);
	
float4 PS( float3 uvwTex : TEXCOORD0 ) : COLOR
{
	//float4 normUvw = float4(normalize(uvwTex), 1.0);
	float4 normUvw = float4(uvwTex, 1.0);
	normUvw.w = 2.0;
	return texCUBEbias( cubeTex, normUvw);
	//return texCUBElod( cubeTex, normUvw);
	//return texCUBE( cubeTex, normUvw);
}
