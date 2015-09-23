/******************************************************************************
Copyright (c) 2015, Geoffrey TOURON
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of dusty nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"0
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#pragma OPENCL EXTENSION cl_amd_printf : enable

typedef struct	Aabb
{
	float4		bottom;
	float4		top;
}	Aabb;

typedef struct	Camera
{
	float4		position;
	float4		forward;
	float4		right;
	float4		up;
	float2		half_resolution;
}	Camera;

typedef struct	Node
{
	int			parent;
	int			left;
	int			right;
	int			height;
	Aabb		aabb;
}	Node;

typedef struct	Material
{
	float4		color;
	float		reflection;
	float		shining;
}	Material;

typedef struct	Light
{
	float4		position;
	float4		color;
}	Light;

///////////////////////////////////////////////////////////////////////////////

typedef struct	Ray
{
	float4		origin;
	float4		direction;
	float4		invdirection;
}	Ray;

typedef struct	Impact
{
	float4		normal;
	float		near;
	float		far;
	int			index;
}	Impact;

///////////////////////////////////////

bool				intersect_rayaabb(Ray const *ray, global Aabb const *aabb, float *tnear, float *tfar)
{
	float4 const	tbot = (aabb->bottom - ray->origin) * ray->invdirection;
	float4 const	ttop = (aabb->top - ray->origin) * ray->invdirection;
	float4 const	tmin = min(tbot, ttop);
	float4 const	tmax = max(tbot, ttop);

	*tnear = max(max(tmin.x, tmin.y), tmin.z);
	*tfar = min(min(tmax.x, tmax.y), tmax.z);

	return (*tfar >= 0 && *tnear <= *tfar);
}

bool				intersect_rayaabb_n(Ray const *ray, global Aabb const *aabb, float *tnear, float *tfar, float4 *normal)
{
	float4 const	tbot = (aabb->bottom - ray->origin) * ray->invdirection;
	float4 const	ttop = (aabb->top - ray->origin) * ray->invdirection;
	float4 const	tmin = min(tbot, ttop);
	float4 const	tmax = max(tbot, ttop);

	if (tmin.x > tmin.y)
	{
		if (tmin.x > tmin.z)
			*normal = (float4)(ray->invdirection.x >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f, 0.0f);
		else
			*normal = (float4)(0.0f, 0.0f, ray->invdirection.z >= 0.0f ? -1.0f : 1.0f, 0.0f);
	}
	else
	{
		if (tmin.y > tmin.z)
			*normal = (float4)(0.0f, ray->invdirection.y >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f);
		else
			*normal = (float4)(0.0f, 0.0f, ray->invdirection.z >= 0.0f ? -1.0f : 1.0f, 0.0f);
	}
	*tnear = max(max(tmin.x, tmin.y), tmin.z);
	*tfar = min(min(tmax.x, tmax.y), tmax.z);

	return (*tfar >= 0 && *tnear <= *tfar);
}

bool	find_closest(int const root, global Node const *nodes, Ray const *ray, Impact *data)
{
	if (root == -1)
		return (false);

	int		stack[32];
	float	near_stack[32];
	int		top = 0;

	data->index = -1;
	data->near = INFINITY;
	stack[top] = root;
	near_stack[top++] = 0.0f;
	do
	{
		if (near_stack[--top] < data->near)
		{
			int const	index = stack[top];

			if (nodes[index].right == -1)
			{
				data->index = index;
				data->near = near_stack[top];
			}
			else
			{
				float	lnear;
				float	lfar;
				float	rnear;
				float	rfar;

				int const	left = nodes[index].left;
				int const	right = nodes[index].right;
				bool const	ltrue = intersect_rayaabb(ray, &nodes[left].aabb, &lnear, &lfar);
				bool const	rtrue = intersect_rayaabb(ray, &nodes[right].aabb, &rnear, &rfar);

				if (lnear > rnear)
				{
					if (ltrue)
					{
						near_stack[top] = lnear;
						stack[top++] = left;
					}
					if (rtrue)
					{
						near_stack[top] = rnear;
						stack[top++] = right;
					}
				}
				else
				{
					if (rtrue)
					{
						near_stack[top] = rnear;
						stack[top++] = right;
					}
					if (ltrue)
					{
						near_stack[top] = lnear;
						stack[top++] = left;
					}
				}
			}
		}
	}
	while (top);

	if (data->index != -1)
	{
		intersect_rayaabb_n(ray, &nodes[data->index].aabb, &data->near, &data->far, &data->normal);
		return (true);
	}
	else
		return (false);
}

bool	check_light(int const root, global Node const *nodes, Ray const *ray, float const distance, int const idx)
{
	if (root != -1)
	{
		int	stack[32];
		int	top = 0;

		stack[top++] = root;
		do
		{
			int		index = stack[--top];
			float	near;
			float	far;

			if (intersect_rayaabb(ray, &nodes[index].aabb, &near, &far) && near < distance)
			{
				if (nodes[index].right == -1)
				{
					if (index != idx)
						return (false);
				}
				else
				{
					stack[top++] = nodes[index].left;
					stack[top++] = nodes[index].right;
				}
			}
		}
		while (top);
	}
	return (true);
}

void				compute_ray(Camera const *camera, unsigned int const x, unsigned int const y, Ray *ray)
{
	ray->origin = camera->position;
	ray->direction = normalize(camera->forward + camera->right * (x - camera->half_resolution.x) + camera->up * (y - camera->half_resolution.y));
	ray->invdirection = 1.0f / ray->direction;
}

kernel void	raytrace(	Camera const			*camera,
						int const				root,
						global Node const		*nodes,
						unsigned int const		materials_count,
						global Material const	*materials,
						unsigned int const		lights_count,
						global Light const		*lights,
						write_only image2d_t	image)
{
	unsigned int const	x = get_global_id(0);
	unsigned int const	y = get_global_id(1);

	float4	color;
	Impact	impact;
	Ray		ray;
	
	compute_ray(camera, x, y, &ray);
	
	if (find_closest(root, nodes, &ray, &impact))
	{
		ray.origin += ray.direction * impact.near;
		float4	accumulated_color = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
		for (unsigned int i = 0; i < lights_count; ++i)
		{
			ray.direction = lights[i].position - ray.origin;
			float const	distance = length(ray.direction);
			ray.direction /= distance;
			ray.invdirection = 1.0f / ray.direction;
			//float const received_power = dot(ray.direction.xyz, impact.normal.xyz) * (lights[i].color.w / (distance));
			float const received_power = dot(impact.normal, ray.direction);
			
			if (received_power > 0.0f && check_light(root, nodes, &ray, distance, impact.index))
			{
				accumulated_color.xyz += lights[i].color.xyz * received_power;
				accumulated_color.w += received_power;
			}
		}
		//accumulated_color /= accumulated_color + (float4)(0.1f, 0.1f, 0.1f, 0.1f); //vec3(1.0) - exp(-hdrColor * exposure);
		color = materials[nodes[impact.index].left].color * (accumulated_color + 0.1f);
		//color = fabs(impact.normal) * (accumulated_color + 0.2f);
	}
	else
		color = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	//write_imagef(image, (int2)(x, y), color);
	color *= 255.0f;
	write_imageui(image, (int2)(x, y), (uint4)(color.x, color.y, color.z, color.w));
}
