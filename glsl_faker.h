// Advance this some day... bascially structs which make it a bit easier to test glsl code on cpu
// Add as necessary
// Can't do it all obviously (no sizzling, no dfdx etc), but at least basic vector stuff should be possible
// Or maybe look around - pretty sure someone did that already (there was one math lib I think which does all this which many engines use...)

#include <cmath>

struct vec2 
{
	float x,y;

	vec2() : x(0), y(0) {}
	vec2(float x_) : x(x_), y(x_) {}
	vec2(float x_, float y_) : x(x_), y(y_) {}

	const vec2& operator*=(const vec2& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	vec2 operator+(const vec2& other)
	{
		return vec2(x + other.x, y + other.y);
	}

	vec2 operator-(const vec2& other)
	{
		return vec2(x - other.x, y - other.y);
	}

	vec2 operator*(const vec2& other)
	{
		return vec2(x * other.x, y * other.y);
	}

	vec2 operator/(const vec2& other)
	{
		return vec2(x / other.x, y / other.y);
	}

	const vec2& operator+=(float s)
	{
		x += s;
		y += s;
		return *this;
	}

	const vec2& operator-=(float s)
	{
		x -= s;
		y -= s;
		return *this;
	}

	vec2 operator*(float val) const
	{
		return vec2(x * val, y * val);
	}

	vec2 operator/(float val) const
	{
		return vec2(x / val, y / val);
	}

	vec2 operator+(float val) const
	{
		return vec2(x + val, y + val);
	}

	vec2 operator-(float val) const
	{
		return vec2(x - val, y - val);
	}
};

vec2 operator+(float val, const vec2& rhs) { return rhs + val; }
vec2 operator-(float val, const vec2& rhs) { return vec2(val-rhs.x, val-rhs.y); }
vec2 operator*(float val, const vec2& rhs) { return rhs * val; }
vec2 operator/(float val, const vec2& rhs) { return vec2(val/rhs.x, val/rhs.y); }

struct vec3
{
	float x,y,z;

	vec3() : x(0), y(0), z(0) {}
	vec3(float x_) : x(x_), y(x_), z(x_) {}
	vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_)  {}

	float& operator[](size_t index)
	{
		switch (index)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: return z;
		}
	}

	vec3 operator+(const vec3& other) const
	{
		return vec3(x + other.x, y + other.y, z + other.z);
	}

	vec3 operator-(const vec3& other) const
	{
		return vec3(x - other.x, y - other.y, z - other.z);
	}

	vec3 operator*(float val) const
	{
		return vec3(x * val, y * val, z * val);
	}

	vec3 operator/(float val) const
	{
		return vec3(x / val, y / val, z / val);
	}

	vec3 operator+(float val) const
	{
		return vec3(x + val, y + val, z + val);
	}

	vec3 operator-(float val) const
	{
		return vec3(x - val, y - val, z - val);
	}
};

vec3 operator+(float val, const vec3& rhs) { return rhs + val; }
vec3 operator-(float val, const vec3& rhs) { return vec3(val-rhs.x, val-rhs.y, val-rhs.z); }
vec3 operator*(float val, const vec3& rhs) { return rhs * val; }
vec3 operator/(float val, const vec3& rhs) { return vec3(val/rhs.x, val/rhs.y, val/rhs.z); }

struct vec4
{
	float x,y,z,w;

	vec4() : x(0), y(0), z(0), w(0) {}
	vec4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_)  {}

	float& operator[](size_t index)
	{
		switch (index)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		default: return w;
		}
	}

	const vec4& operator/=(float d)
	{
		x /= d;
		y /= d;
		z /= d;
		w /= d;
		return *this;
	}
};

struct ivec2
{
	int x, y;
	ivec2() : x(0), y(0) {}
	ivec2(int x_, int y_) : x(x_), y(y_) {}
	ivec2(const vec2& v) : x((int)v.x), y((int)v.y) {}

	bool operator!=(const ivec2& other) const
	{
		return x != other.x || y != other.y;
	}
};

struct uvec4
{
	unsigned int x,y,z,w;

	unsigned int& operator[](size_t index)
	{
		switch (index)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		default: return w;
		}
	}
};

float length(const vec2& v)
{
	return sqrtf( v.x*v.x + v.y*v.y );
}

float length(const vec3& v)
{
	return sqrtf( v.x*v.x + v.y*v.y + v.z*v.z );
}

float length(const vec4& v)
{
	return sqrtf( v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w );
}

vec3 sqrt(const vec3& v)
{
	return vec3(sqrtf(v.x), sqrtf(v.y), sqrtf(v.z));
}

vec3 log(const vec3& v)
{
	return vec3(logf(v.x), logf(v.y), logf(v.z));
}

vec2 normalize(const vec2& v)
{
	float len = length(v);
	if ( len != 0.f )
	{
		return vec2(v.x/len, v.y/len);
	}
	return v;
}

vec3 normalize(const vec3& v)
{
	float len = length(v);
	if ( len != 0.f )
	{
		return vec3(v.x/len, v.y/len, v.z/len);
	}
	return v;
}

vec4 normalize(const vec4& v)
{
	float len = length(v);
	if ( len != 0.f )
	{
		return vec4(v.x/len, v.y/len, v.z/len, v.w/len);
	}
	return v;
}

float dot(const vec2& v1, const vec2& v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

float dot(const vec3& v1, const vec3& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

float dot(const vec4& v1, const vec4& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
}

vec3 reflect(const vec3& incident, const vec3& normal)
{
	return incident - 2.f * dot(normal, incident) * normal;
}

int min(int a, int b)
{
	return a < b ? a : b;
}

vec2 min(vec2 a, vec2 b)
{
	return vec2(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y);
}

vec3 min(vec3 a, vec3 b)
{
	return vec3(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z);
}

int max(int a, int b)
{
	return a < b ? b : a;
}

vec2 max(vec2 a, vec2 b)
{
	return vec2(a.x < b.x ? b.x : a.x, a.y < b.y ? b.y : a.y);
}

vec3 max(vec3 a, vec3 b)
{
	return vec3(a.x < b.x ? b.x : a.x, a.y < b.y ? b.y : a.y, a.z < b.z ? b.z : a.z);
}
