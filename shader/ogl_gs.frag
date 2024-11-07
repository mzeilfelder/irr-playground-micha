#version 150 compatibility

uniform sampler2D myTexture;
uniform vec4 mEmissive;

in geomData
{
	vec3 color;
	vec2 texCoords;
	flat float distance;
} inData;

void main (void)
{
    vec4 col = texture2D(myTexture, inData.texCoords);
    col.xyz *= inData.color;
    gl_FragColor = col * 4.0;
	gl_FragColor += mEmissive;
	gl_FragColor.a = 1.0 - (inData.distance/100);
}
