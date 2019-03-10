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
            </td>
        </tr>
        <tr>
            <td align="middle">
                <img src="images/p1_bunny.png" width="70%"/>
                <figcaption align="middle"><code>bunny.dae</code> with normal shading.</figcaption>
            </td>
            <td align="middle">
                <img src="images/p1_dragon.png" width="70%"/>
                <figcaption align="middle"><code>dragon.dae</code> with normal shading.</figcaption>
            </td>
        </tr>
    </table>
</div>

## Part 2: Bounding Volume Hierarchy

We construct the bounding volume hierarchy (BVH) by recursively splitting the primitive list into two children. If the number of primitives is larger than `max_leaf_size`, we split the primitives using the heuristic of median, since it tries to split the primitives into two part with same number of items in both children. Then we recursively call the construction function on the two splitted list. If the number is small enough, we create a leaf node with the list of primitives in the node. This gives us the hierarchical tree of primitives we want.

If we want to do an intersection test on a BVH node, we first test if the ray intersects with the bounding box of the node. If not, we can safely assert that the ray does not intersect with any of the primitives in the node. If intersects, if the is a leaf node, we find the minimum intersection point w.r.t. each of the primitives in the node; otherwise, we find the minimum intersection w.r.t the two child nodes, if exists.

<div align="middle">
    <table width="100%" align="middle">
        <tr>
            <td align="middle">
                <img src="images/p2_maxplanck.png" width="70%"/>
                <figcaption align="middle"><code>maxplanck.dae</code>, 0.0633s BVH building, 0.0713s rendering.</figcaption>
            </td>
            <td align="middle">
                <img src="images/p2_lucy.png" width="70%"/>
                <figcaption align="middle"><code>CBlucy.dae</code>, 0.2317s BVH building, 0.0588s rendering.</figcaption>
            </td>
        </tr>
    </table>
</div>

The following table compares the runtime of the  naive primitive list and the VBH:

<div align="middle">
    <table width="100%" align="middle">
        <tr>
            <td> 
                <code> Input scene file: ../dae/meshedit/cow.dae </code> 
            </td>
            <td> 
                <code> Input scene file: ../dae/meshedit/cow.dae </code> 
            </td>
        </tr>
        <tr>
            <td>
                <code> Collecting primitives... Done! (0.0008 sec)  </code>
            </td>
            <td>
                <code> Collecting primitives... Done! (0.0016 sec)  </code>
            </td>
        </tr>
        <tr>
            <td>
                <code> Building BVH from 5856 primitives... Done! (0.0001 sec)  </code>
            </td>
            <td>
                <code> Building BVH from 5856 primitives... Done! (0.0120 sec)  </code>
            </td>
        </tr>
        <tr>
            <td>
                <code> Rendering... 100%! (74.3638s)  </code>
            </td>
            <td>
                <code> Rendering... 100%! (0.8139s)  </code>
            </td>
        </tr>
        <tr>
            <td>
                <code> BVH traced 1872935 rays.  </code>
            </td>
            <td>
                <code> BVH traced 1647410 rays.  </code>
            </td>
        </tr>
        <tr>
            <td>
                <code> Averaged 742.070462 intersection tests per ray.  </code>
            </td>
            <td>
                <code> Averaged 2.970438 intersection tests per ray.  </code>
            </td>
        </tr>
    </table>
</div>

VBH takes about 120x time to build the tree, but it negligible in comparision to the 90x time improvement for the much longer rendering time. This is due to the fact that each ray only takes 3 intersection test in comparison to the naive method's 700 intersection tests. The bounding box helps to remove redundant intersection tests on those primitives that are guaranteed not to intersect with the camera ray.

## Part 3: Direct Illumination

In uniform hemisphere sampling, we uniformly sample a number of rays from the hemisphere centered at the hit point, and find the corresponding reflected rays. Then we test if anything hits the reflected rays. If so, we add in the illumination of the first hitting point of the reflected ray and take average of all the sample rays. Note that the illumination from the hitting point must be weighted by the angle and the probability density of the sampling direction.

In importance light sampling, instead of sampling from the hitting point , we sample from each of the light source. For each light, we sample a number of points on it and test if the hitting point can be reflected to the light without anything blocking. If so, we add in the illumination of the light source and take average of all the light rays. Note that the illumination must be weighted by the angle, the probability density of the sample point on the light source, and the probability density of given reflecting angle. this methods converges faster since we refrain from sampling directions that does not hit light source. 

<div align="middle">
    <table width="100%" align="middle">
        <tr>
            <td align="middle">
                <img src="images/p3_bunny_hem_ray_1.png" width="100%"/>
                <figcaption align="middle">
                    Uniform hemisphere sampling <br> 1 light ray per per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p3_bunny_hem_ray_4.png" width="100%"/>
                <figcaption align="middle">
                    Uniform hemisphere sampling <br> 4 light rays per per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p3_bunny_hem_ray_16.png" width="100%"/>
                <figcaption align="middle">
                    Uniform hemisphere sampling <br> 16 light rays per per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p3_bunny_hem_ray_64.png" width="100%"/>
                <figcaption align="middle">
                    Uniform hemisphere sampling <br> 64 light rays per per pixel.
                </figcaption>
            </td>
        </tr>
        <tr>
            <td align="middle">
                <img src="images/p3_bunny_light_ray_1.png" width="100%"/>
                <figcaption align="middle">
                    Importance light sampling <br> 1 light ray per per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p3_bunny_light_ray_4.png" width="100%"/>
                <figcaption align="middle">
                    Importance light sampling <br> 4 light rays per per pixel.
                </figcaption>
                        </td>
            <td align="middle">
                <img src="images/p3_bunny_light_ray_16.png" width="100%"/>
                <figcaption align="middle">
                    Importance light sampling <br> 16 light rays per per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p3_bunny_light_ray_64.png" width="100%"/>
                <figcaption align="middle">
                    Importance light sampling <br> 64 light rays per per pixel.
                </figcaption>
            </td>           
        </tr>
    </table>
</div>

As we can see from the images above, both method converges to the true lighting of the scene. However, light sampling converges much faster as the number of light rays per pixel increased. Also, we can see that different parts of the scene also converges in a different rate. The bottom of the back walls converges quite fast, while the complex part of the bunny and the soft shadows converges relatively slow. This is due to the fact that if there is something is partially blocking a point from receiving the light, importance light sampling might find that some of the samples are blocked and thus the sample becomes useless. 

## Part 4: Global Illumination

The global illumination rendering algorithm is basically recursively apply direct illumination algorithm, and terminate appropriately. For each bounce, we apply a global illumination calculation with correct weighting, and sample a new incoming light ray from the hit point. Then we apply Russian Roulette termination, see if we should terminate or continue to propagate light, if so, we incorporate a new direct light using the new ray and recurse. The base case is that the zero bounce case, where we only consider the light directly comes from the source to the camera. We also have some termination recursion if depth exceed a certain level, and force the algorithm to do at least one bounce if the max depth is greater than 1.

<div align="middle">
    <img src="images/p4_dragon_sample_1024.png" width="70%"/>
    <figcaption align="middle">
        <code>dragon.dae</code> with 64 light rays and 1024 samples per pixel.
    </figcaption>
</div>

<div align="middle">
    <table width="100%" align="middle">
        <tr>
            <td align="middle">
                <img src="images/p4_bunny_global_depth_0.png" width="100%"/>
                <figcaption align="middle"> 
                    Max depth of 0.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_bunny_global_depth_1.png" width="100%"/>
                <figcaption align="middle"> 
                    Max depth of 1.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_bunny_global_depth_2.png" width="100%"/>
                <figcaption align="middle"> 
                    Max depth of 2.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_bunny_global_depth_3.png" width="100%"/>
                <figcaption align="middle"> 
                    Max depth of 3.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_bunny_global_depth_100.png" width="100%"/>
                <figcaption align="middle"> 
                    Max depth of 100.
                </figcaption>
            </td>
        </tr>
        <tr>
            <td align="middle">
                <img src="images/p4_bunny_indirect_depth_0.png" width="100%"/>
                <figcaption align="middle"> 
                    At depth 0.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_bunny_indirect_depth_1.png" width="100%"/>
                <figcaption align="middle"> 
                    At depth 1.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_bunny_indirect_depth_2.png" width="100%"/>
                <figcaption align="middle"> 
                    At depth 2.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_bunny_indirect_depth_3.png" width="100%"/>
                <figcaption align="middle"> 
                    At depth 3.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_bunny_indirect_depth_100.png" width="100%"/>
                <figcaption align="middle"> 
                    From depth 4 to 100.
                </figcaption>
            </td>
        </tr>
    </table>
</div>

The top row shows the global illumination `CBbunny.dae` with 1024 samples per pixel and different `max_ray_depth`, and the lower row only shows the indirect illumination at each depth. Note that:
- at depth 0, we can only see the light since 0 bounce only consider light directly from the light sources; 
- at depth 1, we cannot see the ceiling since the ceiling is behind the light source, it requires at least 2 bounces for a light ray to hit it; 
- as max depth increases, we can gradually see the red or blue diffuse light reflected from the side planes onto the gray ground and back planes and the bunny; 
- if we increase max depth to 100, the increase of lighting is negligible since most rays cannot reach a higher depth due to Russian Roulette termination.

<div align="middle">
    <table width="100%" align="middle">
        <tr>
            <td align="middle">
                <img src="images/p4_spheres_sample_1.png" width="100%"/>
                <figcaption align="middle"> 
                    1 samples per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_spheres_sample_2.png" width="100%"/>
                <figcaption align="middle"> 
                    2 samples per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_spheres_sample_4.png" width="100%"/>
                <figcaption align="middle"> 
                    4 samples per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_spheres_sample_8.png" width="100%"/>
                <figcaption align="middle"> 
                    8 samples per pixel.
                </figcaption>
            </td>
        <tr>
        </tr>
            <td align="middle">
                <img src="images/p4_spheres_sample_16.png" width="100%"/>
                <figcaption align="middle"> 
                    16 samples per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_spheres_sample_32.png" width="100%"/>
                <figcaption align="middle"> 
                    32 samples per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_spheres_sample_64.png" width="100%"/>
                <figcaption align="middle"> 
                    64 samples per pixel.
                </figcaption>
            </td>
            <td align="middle">
                <img src="images/p4_spheres_sample_1024.png" width="100%"/>
                <figcaption align="middle"> 
                    1024 samples per pixel.
                </figcaption>
            </td>
        </tr>
    </table>
</div>

Note that with fewer samples per pixel, it is more likely for a pixel to deviate far from the expected lighting, and there are huge differences between neighbor pixels; but as the sample rate increases, the result converges to the expected output.

## Part 5: Adaptive Sampling

In adaptive sampling, we consider the distribution of the illumination samples. If the variance is small enough or the number of samples is small enough, we conclude that the pixel is well-sampled and converged and thus terminate early. By keeping track of the sum and sum of squares of the illumination of the samples so far, we can compute if the size of the 95% confidence interval is below a certain threshold. If so, we can stop sampling on this pixel.

<div align="middle">
    <table width="100%" align="middle">
        <tr>
            <td align="middle">
                <img src="images/p5_bunny.png" width="70%"/>
            </td>
            <td align="middle">
                <img src="images/p5_bunny_rate.png" width="70%"/>
            </td>
        </tr>
    </table>
    <figcaption align="middle">
                        <code>CBbunny.dae</code> and its corresponding sample rate distribution. Rendered with 2048 samples per pixel, 1 sample per light and a max ray depth of 5.
                    </figcaption>
</div>
