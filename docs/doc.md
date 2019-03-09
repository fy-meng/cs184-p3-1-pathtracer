Assignment 3: PathTracer
====================
Fanyu Meng

## Overview
In this project, we implemented a crude path tracer that can render simple objects. By implementing the `Ray` class and `intersect` functions in some primitives, we allow the camera ray to trace a light path and return the result. Using that abstraction, we implemented direct and indirect illumination, allowing the camera to find the color of a pixel with one bounce or multiple bounces. 

We also include several optimization methods. The first one is to organize the primitives using a hierarchical tree of bounding boxes, which allow us to skip many intersection test if the ray is not intersecting with the box; the second optimization is to incorporating adaptive sampling, stop generating rays if the pixel have already likely converged. 

## Part 1: Ray Generation and Scene Intersection

The structure of the rendering pipeline is:
1. For each sample per pixel, convert the 2D pixel coordinate into 3D ray direction from the camera to the sample location on the imaging plane;
2. Using a radiance calculating method, find the radiance of the given ray. First we find the first intersection on the path of the ray. By scanning through all primitives or scanning in a hierarchical order, we update the intersection point on the line to the first intersection;
3. Using the material BRDF and the reflecting direction of the intersection point, find the illumination of the ray on the given intersection.

The triangle intersection algorithm is basically a barycentric-coordinates test. Using the direction of the ray and the normal vector of the triangle, we can find the hitting point of the ray on the plane of the triangle. By finding the barycentric-coordinates w.r.t. to the triangle, we can apply a `point_in_triangle` test and find the distance of the hitting point of the ray from the camera. We then check if the point is valid (e.g. hitting point is behind the camera) and update the valid region of the ray. 

<div align="middle">
    <table width="100%" align="middle">
        <tr>
            <td align="middle">
                <img src="images/p1_spheres.png" width="70%"/>
                <figcaption align="middle"><code>CBspheres.dae</code> with normal shading.</figcaption>
            </td>
            <td align="middle">
                <img src="images/p1_coil.png" width="70%"/>
                <figcaption align="middle"><code>CBcoil.dae</code> with normal shading.</figcaption>
                                                       </td></figcaption>
            </td>
        </tr>
        <tr>
            <td align="middle">
                <img src="images/p1_bunny.png" width="70%"/>
                <figcaption align="middle"><code>bunny.dae</code> with normal shading.</figcaption>
                                                       </td></figcaption>
            </td>
            <td align="middle">
                <img src="images/p1_dragon.png" width="70%"/>
                <figcaption align="middle"><code>dragon.dae</code> with normal shading.</figcaption>
                                                       </td></figcaption>
            </td>
        </tr>
    </table>
</div>

## Part 2: Bounding Volume Hierarchy


## Part 3: Direct Illumination


## Part 4: Global Illumination


## Part 5: Adaptive Sampling

