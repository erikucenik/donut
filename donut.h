#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char C[] = ".,-~:;=!*#$@";

int index_of(char*s, char a) {for (int i = 0; s[i] != 0; i++) if(s[i]==a) return i; return -1;}

typedef struct {
	float x, y, z;
} V;

typedef struct {
	V Origin, Normal;
} Plane;

typedef struct {
	V center;
	float R1, R2;
} T;

V v(float x, float y, float z)
{
	V d = {x, y, z};
	return d;
}

V cross(V a, V b)
{
	return v(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

V sum(V v1, V v2)
{
	return v(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

V scale(V n, float a)
{
	return v(n.x * a, n.y * a, n.z * a);
}

V am(float m[3][3], V n)
{
	V x = scale(v(m[0][0], m[1][0], m[2][0]), n.x);
	V y = scale(v(m[0][1], m[1][1], m[2][1]), n.y);
	V z = scale(v(m[0][2], m[1][2], m[2][2]), n.z);

	return sum(sum(x, y), z);
}

V subtract(V v1, V v2)
{
	return sum(v1, scale(v2, -1.0));
}

float dot(V v1, V v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

V normalize(V n)
{
	return scale(n, 1.0/sqrt(dot(n, n)));
}

V pj(Plane plane, V point)
{
	V d = subtract(point, plane.Origin);
	float distance = dot(d, plane.Normal);
	V projected_point = subtract(point, scale(plane.Normal, distance));
	return projected_point;
}

V rc(Plane plane, V point)
{
	return subtract(point, plane.Origin);
}

V tp(T torus, float u, float b)
{
	float x = cos(u) * (torus.R1 + torus.R2 * cos(b));
	float y = sin(u) * (torus.R1 + torus.R2 * cos(b));
	float z = sin(b) * torus.R2;

	return sum(v(x, y, z), torus.center);
}

V torus_normal(T torus, float u, float b)
{
	float dx_du = -sin(u) * (torus.R1 + torus.R2 * cos(b));
	float dy_du =  cos(u) * (torus.R1 + torus.R2 * cos(b));
	float dz_du =  0.0;
	V dV_du =  v(dx_du, dy_du, dz_du);

	float dx_dv = -torus.R2 * cos(u) * sin(b);
	float dy_dv = -torus.R2 * sin(u) * sin(b);
	float dz_dv =  torus.R2 * cos(b);
	V dV_dv =  v(dx_dv, dy_dv, dz_dv);

	V normal = normalize(cross(dV_dv, dV_du));
	return normal;
}

float gb(T torus, float alpha, float beta, float rotation_matrix[3][3], V light_source)
{
	V point = am(rotation_matrix, tp(torus, alpha, beta));
	V normal = am(rotation_matrix, torus_normal(torus, alpha, beta));
	V d = normalize(subtract(point, light_source));

	float brightness = dot(d, scale(normal, -1));
	return (brightness < 0) ? 0 : brightness;
}

char b2c(float brightness)
{
	int index = round(brightness*11.0);
	return C[index];
}