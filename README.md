This is a photo-realistic ray tracing renderer written in C++.
## Dependencies

+ Visual Studio 2017
+ glm
+ OpenCV 3.1.0

## Features

+ Monte Carlo Path Tracing & Bidirectional Path Tracing.
+ <a href="https://en.wikipedia.org/wiki/Phong_reflection_model">Phong model</a>.
+ Rendering of infinite plane, sphere and obj mesh model.
+ Reflection & refraction.
+ Orthographic projection & perspective projection.
+ Soft shadow.
+ Anti aliasing by jitter sampling.
+ Depth of field by super sampling.
+ Texture including pure color, grid texture and image texture.
+ Implemented <a href="http://www.eng.utah.edu/~cs6965/papers/kdtree.pdf">SAH-based</a> KD-Tree.
+ Smooth shading.

## Gallery

+ Render two balls in Cornell Box with 100spp by using bidirectional path tracing.
<div style="text-align:center; margin-top:30px"><img src="gallery/bdpt_100_glass_balls.png"></div>

+ Stanford dragon and 10,000 spheres with depth-of-field & anti-aliasing & soft shadow.
<div style="text-align:center; margin-top:30px"><img src="gallery/dragon_dop20_anti20_soft49.png"></div>

+ Two spheres with dop & anti-aliasing & soft shadow.
<div style="text-align:center; margin-top:30px"><img src="gallery/two_balls_dop20_anti20_soft49.png"></div>

+ Several spheres with with dop & anti-aliasing & soft shadow.
<div style="text-align:center; margin-top:30px"><img src="gallery/balls_dop20_anti20_soft49.png"></div>