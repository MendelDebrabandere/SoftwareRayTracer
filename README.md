# SoftwareRayTracer

Github repo: https://github.com/MendelDebrabandere/SoftwareRayTracer

A software ray tracer made fully in C++ that uses multithreading

![raytracer](https://github.com/MendelDebrabandere/SoftwareRayTracer/assets/95921047/8067c9d1-d732-4db1-86b8-6bef7d9d9516)

## Keybinds
F2: Toggle shadwos (helps a lot with fps)\
F3: Cycle lighting mode (Combined -> ObservedArea -> Radiance -> BRDF) \
F6: Start fps benchmark of 10 frames \
\
WASD, EQ: Full 3D movement\
Right mouse button: Pan camera

## FPS
![image](https://github.com/MendelDebrabandere/SoftwareRayTracer/assets/95921047/a7a7f336-a36b-4349-b085-924fe3ef3b11)

On my pc I get an average of about 82 fps when it is on combined lighting with shadows on which is pretty good in my opinion.
I used multithreading in order to get a decent framerate because I used to be stuck at less than 10 fps.

## Code
The way the raytracer works is that for every frame, for every pixel, a ray is created in 3D space. The program then checks if it interacts with any of the geometry in my scene (triangle, sphere or plane).
If it collides with some geometry it calculates the lighting data of that object using Observed Area, Radiance, BRDF, and shadow. \
The high level overview of the RenderPixel function can be found in Renderer.cpp:\
![image](https://github.com/MendelDebrabandere/SoftwareRayTracer/assets/95921047/177d4e21-ab32-4273-94a1-7b9c58f4e198)
