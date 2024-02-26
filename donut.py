import numpy as np
import time
import os

class Plane:
    def __init__(self, Origin, Normal):
        self.Normal = Normal / np.linalg.norm(Normal)
        self.Origin = Origin

    def project(self, point):
        v = point - self.Origin
        distance = v.dot(self.Normal)
        projected_point = point - distance * self.Normal

        return projected_point

    def relative_coords(self, point):
        return point - self.Origin

class Torus:
    def __init__(self, R, r, center):
        self.R = R
        self.r = r
        self.center = center

    def point_at(self, u, v):
        x = np.cos(u) * (self.R + self.r * np.cos(v))
        y = np.sin(u) * (self.R + self.r * np.cos(v))
        z = np.sin(v) * self.r

        return np.array([x, y, z]) + self.center

    def normal_at(self, u, v):
        dx_du = -np.sin(u) * (self.R + self.r * np.cos(v))
        dy_du =  np.cos(u) * (self.R + self.r * np.cos(v))
        dz_du =  0
        dV_du = np.array([dx_du, dy_du, dz_du])

        dx_dv = -self.r * np.cos(u) * np.sin(v)
        dy_dv = -self.r * np.sin(u) * np.sin(v)
        dz_dv =  self.r * np.cos(v)
        dV_dv = np.array([dx_dv, dy_dv, dz_dv])

        normal = np.cross(dV_dv, dV_du)
        normal /= np.linalg.norm(normal)

        return normal

def brightness_at(torus, u, v, rotation_matrix, light_source):
    point = rotation_matrix @ torus.point_at(u, v)
    normal = rotation_matrix @ torus.normal_at(u, v)
    d = point - light_source
    d = d / np.linalg.norm(d)

    brightness = np.dot(d, -normal)

    if brightness < 0:
        return 0
    else:
        return brightness

def brightness_to_character(brightness):
    characters = ".,-~:;=!*#$@"
    index = round(brightness * 11)
    return characters[index]

def print_screen(screen):
    os.system("clear")
    for row in screen:
        print("|", end='')
        for (_, col) in row:
            print(col, end='')
        print("|\n", end='')

def project(point):
    return np.array([point[0], 0, point[2]])

def plane_coords_to_terminal_coords(plane_coords):
    col = int(COLS/2 + plane_coords[0])
    row = int(ROWS/2 - plane_coords[2])

    return (row, col)

def create_screen(ROWS, COLS):
    screen = []

    for _ in range(ROWS):
        row = []

        for _ in range(COLS):
            row.append( (0, ' ') )

        screen.append(row)

    return screen

def main(ROWS, COLS, torus, plane, light_source):
    theta = 0

    screen = create_screen(ROWS, COLS)
    while True:
        c = np.cos(theta)
        s = np.sin(theta)
        rotation_matrix = np.array([[1, 0, 0], [0, c, -s], [0, s,  c]])

        for u in np.linspace(0, 2*np.pi, 100):
            for v in np.linspace(0, 2*np.pi, 100):
                torus_point = rotation_matrix @ torus.point_at(u, v)
                projected_point = plane.project(torus_point)
                projected_point = plane.relative_coords(projected_point)
                brightness = brightness_at(torus, u, v, rotation_matrix, light_source)

                distance = abs(torus_point[1])
                character = brightness_to_character(brightness)
                (row, col) = plane_coords_to_terminal_coords(projected_point)

                screen[row][col] = (distance, character) 

        theta += 0.1
        print_screen(screen)

if __name__ == "__main__":
    R = 15
    r = 7
    COLS = ROWS = (R + 2*r) * 2
    torus = Torus(R, r, np.array([0, 6, 0]))
    plane = Plane(np.array([0,0,0]), np.array([0,1,0]))

    light_source = np.array([3, 6, 0])

    main(ROWS, COLS, torus, plane, light_source)
