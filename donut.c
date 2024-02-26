#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char characters[] = ".,-~:;=!*#$@";

int index_of(char* str, char a)
{
	for (int index = 0; str[index] != 0; index++) {
		if (str[index] == a)
			return index;
	}

	return -1;
}

typedef struct {
	float x, y, z;
} Vec3;

typedef struct {
	Vec3 Origin, Normal;
} Plane;

typedef struct {
	Vec3 center;
	float R1, R2;
} Torus;

Vec3 vec3(float x, float y, float z)
{
	Vec3 v = {x, y, z};
	return v;
}

Vec3 cross(Vec3 a, Vec3 b)
{
	return vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

Vec3 sum(Vec3 v1, Vec3 v2)
{
	return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vec3 scale(Vec3 v, float a)
{
	return vec3(v.x * a, v.y * a, v.z * a);
}

Vec3 apply_matrix(float m[3][3], Vec3 v)
{
	Vec3 x = scale(vec3(m[0][0], m[1][0], m[2][0]), v.x);
	Vec3 y = scale(vec3(m[0][1], m[1][1], m[2][1]), v.y);
	Vec3 z = scale(vec3(m[0][2], m[1][2], m[2][2]), v.z);

	return sum(sum(x, y), z);
}

Vec3 subtract(Vec3 v1, Vec3 v2)
{
	return sum(v1, scale(v2, -1.0));
}

float dot(Vec3 v1, Vec3 v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vec3 normalize(Vec3 v)
{
	return scale(v, 1.0/sqrt(dot(v, v)));
}

Vec3 project(Plane plane, Vec3 point)
{
	Vec3 v = subtract(point, plane.Origin);
	float distance = dot(v, plane.Normal);
	Vec3 projected_point = subtract(point, scale(plane.Normal, distance));
	return projected_point;
}

Vec3 relative_coords(Plane plane, Vec3 point)
{
	return subtract(point, plane.Origin);
}

Vec3 torus_point(Torus torus, float u, float v)
{
	float x = cos(u) * (torus.R1 + torus.R2 * cos(v));
	float y = sin(u) * (torus.R1 + torus.R2 * cos(v));
	float z = sin(v) * torus.R2;

	return sum(vec3(x, y, z), torus.center);
}

Vec3 torus_normal(Torus torus, float u, float v)
{
	float dx_du = -sin(u) * (torus.R1 + torus.R2 * cos(v));
	float dy_du =  cos(u) * (torus.R1 + torus.R2 * cos(v));
	float dz_du =  0.0;
	Vec3 dV_du =  vec3(dx_du, dy_du, dz_du);

	float dx_dv = -torus.R2 * cos(u) * sin(v);
	float dy_dv = -torus.R2 * sin(u) * sin(v);
	float dz_dv =  torus.R2 * cos(v);
	Vec3 dV_dv =  vec3(dx_dv, dy_dv, dz_dv);

	Vec3 normal = normalize(cross(dV_dv, dV_du));
	return normal;
}

float get_brightness(Torus torus, float alpha, float beta, float rotation_matrix[3][3], Vec3 light_source)
{
	Vec3 point = apply_matrix(rotation_matrix, torus_point(torus, alpha, beta));
	Vec3 normal = apply_matrix(rotation_matrix, torus_normal(torus, alpha, beta));
	Vec3 d = normalize(subtract(point, light_source));

	float brightness = dot(d, scale(normal, -1));
	return (brightness < 0) ? 0 : brightness;
}

char brightness_to_character(float brightness)
{
	int index = round(brightness*11.0);
	return characters[index];
}

void print_torus(Torus torus, int COLS, int ROWS, float theta, Vec3 light_source, Plane plane)
{
	char *screen = malloc(COLS * ROWS);
	// ojo, que el ' ' lo cuenta como int.
	memset((void*)screen, ' ', ROWS*COLS);
	float c = cos(theta);
	float s = sin(theta);

/*
	float rotation_matrix_x[3][3] = {{1, 0, 0}, {0, c, -s}, {0, s, c}};
	float rotation_matrix_y[3][3] = {{c, 0, s}, {0, 1, 0}, {-s, 0, c}};
	float rotation_matrix_z[3][3] = {{c, -s, 0}, {s, c, 0}, {0, 0, 1}};
*/

	float rotation_matrix[3][3] = {{c, 0, s}, {s*s, c, -c*s}, {-c*s, s, c*c}};

	for (float alpha = 0; alpha < 6.28; alpha += 0.0314) {
		for (float beta = 0; beta < 6.28; beta += 0.0314) {
			Vec3 point = torus_point(torus, alpha, beta);
			point = apply_matrix(rotation_matrix, point);
			Vec3 projection = project(plane, point);
			Vec3 coords_in_plane = relative_coords(plane, projection);
			int col = (int)(COLS/2.0 + coords_in_plane.x);
			int row = (int)(ROWS/2.0 - coords_in_plane.y);
			float brightness = get_brightness(torus, alpha, beta, rotation_matrix, light_source);
			char new_character = brightness_to_character(brightness);

			int new_index = round(brightness * 11.0);
			int old_index = index_of(characters, screen[row*COLS + col]);

			if (old_index == -1 || new_index > old_index)
				screen[row*COLS + col] = new_character;
		}
	}

	printf("\e[1;1H\e[2J");

	for (int row = 0; row < ROWS; row++) {
		putchar('|');
		for (int col = 0; col < COLS; col++)
			putchar(screen[row*COLS + col]);
		
		printf("|\n");
	}
}

int main()
{
	int R1 = 15;
	int R2 = 7;
	int ROWS = (R1 + 2*R2) * 2;
	int COLS = ROWS;

	Vec3 center = vec3(0, 0, 3);
	Vec3 origin = vec3(0, 0, 0);
	Vec3 normal = vec3(0, 0, 1);
	Vec3 light_source = vec3(0, 3, 1);

	Torus torus = {center, R1, R2};
	Plane plane = {origin, normal};

	for (float theta = 0; ; theta += 0.314) {
		print_torus(torus, COLS, ROWS, theta, light_source, plane);
		usleep(50 * 1000);
	}
}