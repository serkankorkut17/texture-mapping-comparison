from PIL import Image, ImageDraw
import numpy as np

class Vertice:
    def __init__(self, x, y, rgb, alpha, z):
        self.x = x
        self.y = y
        self.rgb = rgb
        self.a = alpha
        self.z = z

class Triangle:
    def __init__(self, v1, v2, v3):
        self.v1 = v1
        self.v2 = v2
        self.v3 = v3

v1 = Vertice(300,450,[255,0,0],0.1,30)
v2 = Vertice(110,320,[0,255,0],0.9,50)
v3 = Vertice(0,45,[0,0,255],0.8,10)

v4 = Vertice(30,80,[250,25,25],0.4,10)
v5 = Vertice(240,60,[24,240,240],0.6,50)
v6 = Vertice(400,499,[90,60,90],0.8,300)

v7 = Vertice(405,340,[220,0,210],0.3,5)
v8 = Vertice(440,20,[30,150,20],0.9,40)
v9 = Vertice(350,460,[128,128,128],0.5,20)

v10 = Vertice(20,450,[250,250,250],0.7,25)
v11 = Vertice(145,15,[25,240,750],0.2,60)
v12 = Vertice(140,230,[0,0,128],0.4,200)

v13 = Vertice(350,130,[200,0,0],0.6,45)
v14 = Vertice(34,45,[0,165,50],0.3,15)
v15 = Vertice(350,20,[140,70,0],0.8,0)
triangles = [Triangle(v1,v2,v3),Triangle(v4,v5,v6),Triangle(v7,v8,v9),Triangle(v10,v11,v12),Triangle(v13,v14,v15)]


def barycentric_coordinates(x, y, v1, v2, v3):
    np.float64
    A = np.array([[v3.x-v1.x, v2.x-v1.x],
                  [v3.y-v1.y, v2.y-v1.y]])
    A_inverse = np.linalg.inv(A)
    B = np.array([[x-v1.x],
                  [y-v1.y]])
    
    gamma, beta = np.matmul(A_inverse,B)
    gamma, beta = gamma[0], beta[0]
    alpha = 1 - gamma - beta

    return alpha, beta, gamma

def interpolate_color(alpha, beta, gamma, v1, v2, v3):
    [r1, g1, b1], a1, z1 = v1.rgb, v1.a, v1.z
    [r2, g2, b2], a2, z2 = v2.rgb, v2.a, v2.z
    [r3, g3, b3], a3, z3 = v3.rgb, v3.a, v3.z

    r = int(alpha * r1 + beta * r2 + gamma * r3)
    g = int(alpha * g1 + beta * g2 + gamma * g3)
    b = int(alpha * b1 + beta * b2 + gamma * b3)
    a = float(alpha * a1 + beta * a2 + gamma * a3)
    z = int(alpha * z1 + beta * z2 + gamma * z3)

    return r, g, b, a, z

def draw_triangle(v1,v2,v3):
    draw = ImageDraw.Draw(image)

    x_min = min(v1.x, v2.x, v3.x)
    x_max = max(v1.x, v2.x, v3.x)
    y_min = min(v1.y, v2.y, v3.y)
    y_max = max(v1.y, v2.y, v3.y)

    for x in range(x_min, x_max + 1):  
        for y in range(y_min, y_max + 1):
            alpha, beta, gamma = barycentric_coordinates(x, y, v1,v2,v3)
            if 0 <= alpha <= 1 and 0 <= beta <= 1 and 0 <= gamma <= 1:
                r,g,b,a,z = interpolate_color(alpha, beta, gamma, v1,v2,v3)
                if pixels[x][y] == 0:
                    pixels[x][y] = [r,g,b,z]
                    interpolated_color = (r,g,b)
                    draw.point((x, y), fill=interpolated_color)
                else:
                    r_old, g_old, b_old, z_old = pixels[x][y]
                    if z < z_old:
                        r = int(r*a+r_old*(1-a))
                        g = int(g*a+g_old*(1-a))
                        b = int(b*a+b_old*(1-a))
                        interpolated_color = (r, g, b)
                        pixels[x][y] = [r,g,b,z]
                        draw.point((x, y), fill=interpolated_color)
                    else:
                        continue
    return image

def sorting(v1, v2, v3):
    x_centre = (v1.x + v2.x + v3.x) / 3
    y_centre = (v1.y + v2.y + v3.y) / 3
    alpha, beta, gamma = barycentric_coordinates(x_centre, y_centre, v1,v2,v3)
    r,g,b,a,z = interpolate_color(alpha, beta, gamma, v1,v2,v3)
    return z

width = 500
height = 500
pixels = [ [0]*height for i in range(width)]

image = Image.new("RGBA", (width, height), (0, 0, 0, 255))
triangles.sort(key=lambda p: sorting(p.v1,p.v2,p.v3), reverse=True)

for triangle in triangles:
        draw_triangle(triangle.v1, triangle.v2, triangle.v3)

image.save("output.png")