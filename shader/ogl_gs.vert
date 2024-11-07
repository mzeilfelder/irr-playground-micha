// Version must be first line in shader (aside from comments and whitespace)
// It tells compiler the minimal glsl version it requires
// The other option is "compatibility" or "core" (which is default).
// With core several variables like gl_MultiTexCoord0 are no longer automatically available, 
// but the have to passed instead using uniforms (but don't have to be passed, so things can get faster)
// While we need only 1.5 support for this example it's often nice to have higher requirement for many modern features.
#version 150 compatibility

uniform mat4 mWorld;
uniform mat4 mView;
uniform vec3 mLightPos;	// actually just camera-pos in this case
uniform vec4 mLightColor;

// Interface blocks
// All values are per vertex and have to go through geometry shader
// (there is no way to send values directly to the fragment shader)
// There must be an identical "in" struct in the geometry shader
// And the "out" interface of the geometry shader must fit the "in" of the fragment shader.
// 
// It is possible to set variables in another ways. 
// By name directly like:
// out vec2 texCoords; 
// So vertex out names same as geometry in names. And geometry out names same as fragment shader in names.
// 
// Or via location indices (so name no longer matters), like:
// layout(location = 0) out vec2 texCoords;
// Each varible needs it's own location. And same as with names vertex-out and geometry-in indices must match
// and geometry-out and fragment-in indices.
out VertexData
{
  vec3 color;
  vec2 texCoords;
  vec4 normal;
} outData;

void main(void)
{
	vec4 worldpos = mWorld * gl_Vertex;

	// Note: We stay in world space, not yet using projection matrix, that's done in the geometry shader
	// While this is not always necessary it often makes the geometry shader easier to write
	gl_Position = mView * worldpos;	
	
	mat3 normalMatrix = mat3(transpose(inverse(mView * mWorld)));
	outData.normal = vec4(normalize(normalMatrix * gl_Normal), 0.0);	// also in world space
	outData.texCoords = gl_MultiTexCoord0.xy;
	
	// Stuff below from old Irrlicht shader example to calculate some nice color
	// It's not real lighting, just shiny stuff.
	vec4 lightVector = normalize(worldpos - vec4(mLightPos,1.0));
	
	outData.color = (mLightColor * clamp(dot(-lightVector, outData.normal), 0.1, 1)).xyz;
}
