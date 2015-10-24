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

#define	SEARCHSTACKSIZE 24
#define RAYSTACKSIZE 4

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
	float		refraction;
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
}	Ray;

typedef struct	Impact
{
	float4		normal;
	float		near;
	float		far;
	int			index;
}	Impact;

///////////////////////////////////////////////////////////////////////////////

inline bool			intersect_rayaabb(Ray const *ray, global Aabb const *aabb, float *tnear)
{
	float4 const	tbot = (aabb->bottom - ray->origin) * ray->direction;
	float4 const	ttop = (aabb->top - ray->origin) * ray->direction;
	float4 const	tmin = min(tbot, ttop);
	float4 const	tmax = max(tbot, ttop);

	*tnear = max(max(tmin.x, tmin.y), tmin.z);
	float const tfar = min(min(tmax.x, tmax.y), tmax.z);

	return (tfar >= 0.0f && *tnear <= tfar);
}

inline bool			intersect_rayaabb_n(Ray const *ray, global Aabb const *aabb, float *tnear, float4 *normal)
{
	float4 const	tbot = (aabb->bottom - ray->origin) * ray->direction;
	float4 const	ttop = (aabb->top - ray->origin) * ray->direction;
	float4 const	tmin = min(tbot, ttop);
	float4 const	tmax = max(tbot, ttop);

	if (tmin.x > tmin.y)
	{
		if (tmin.x > tmin.z)
			*normal = (float4)(ray->direction.x >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f, 0.0f);
		else
			*normal = (float4)(0.0f, 0.0f, ray->direction.z >= 0.0f ? -1.0f : 1.0f, 0.0f);
	}
	else
	{
		if (tmin.y > tmin.z)
			*normal = (float4)(0.0f, ray->direction.y >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f);
		else
			*normal = (float4)(0.0f, 0.0f, ray->direction.z >= 0.0f ? -1.0f : 1.0f, 0.0f);
	}
	*tnear = max(max(tmin.x, tmin.y), tmin.z);
	float const	tfar = min(min(tmax.x, tmax.y), tmax.z);

	return (tfar >= 0.0f && *tnear <= tfar);
}

bool	find_closest(int const root, global Node const *nodes, Ray const *ray, Impact *data, int const idx)
{
	if (root == -1)
		return (false);

	Ray const	invray = { ray->origin, 1.0f / ray->direction };
	int			stack[SEARCHSTACKSIZE];
	float		near_stack[SEARCHSTACKSIZE];
	int			top = 0;

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
				if (index != idx)
				{
					data->index = index;
					data->near = near_stack[top];
				}
			}
			else
			{
				float	lnear;
				float	rnear;

				int const	left = nodes[index].left;
				int const	right = nodes[index].right;
				bool const	ltrue = intersect_rayaabb(&invray, &nodes[left].aabb, &lnear);
				bool const	rtrue = intersect_rayaabb(&invray, &nodes[right].aabb, &rnear);

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

	return (data->index != -1 ? intersect_rayaabb_n(&invray, &nodes[data->index].aabb, &data->near, &data->normal) : false);
}

float		find_closest_s(int const root, global Node const *nodes, Ray const *ray, int const value, int *idx)
{
	float	near = INFINITY;
	*idx = -1;
	
	if (root == -1)
		return (near);

	Ray const	invray = { ray->origin, 1.0f / ray->direction };
	int			stack[SEARCHSTACKSIZE];
	float		near_stack[SEARCHSTACKSIZE];
	int			top = 0;

	stack[top] = root;
	near_stack[top++] = 0.0f;
	do
	{
		if (near_stack[--top] < near)
		{
			int const	index = stack[top];

			if (nodes[index].right == -1)
			{
				*idx = index;
				near = near_stack[top];
			}
			else
			{
				float	lnear;
				float	rnear;

				int const	left = nodes[index].left;
				int const	right = nodes[index].right;
				bool const	ltrue = nodes[left].right == -1 && nodes[left].left == value ? false : intersect_rayaabb(&invray, &nodes[left].aabb, &lnear);
				bool const	rtrue = nodes[right].right == -1 && nodes[right].left == value ? false : intersect_rayaabb(&invray, &nodes[right].aabb, &rnear);

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

	if (*idx == -1)
		intersect_rayaabb(&invray, &nodes[root].aabb, &near);
	return (near);
}

bool		compute_shadow(int const root, global Node const *nodes, Ray const *ray, float const distance, int const idx)
{	
	if (root != -1)
	{
		Ray const	invray = { ray->origin, 1.0f / ray->direction };
		int			stack[SEARCHSTACKSIZE];
		int			top = 0;

		stack[top++] = root;
		do
		{
			int const	index = stack[--top];
			float		near;

			if (intersect_rayaabb(&invray, &nodes[index].aabb, &near) && near < distance)
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

///////////////////////////////////////////////////////////////////////////////

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

	float4	color = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	float	coefstack[RAYSTACKSIZE];
	Ray		raystack[RAYSTACKSIZE];
	int		avoidstack[RAYSTACKSIZE];
	int		top = 0;
	int		maxray = RAYSTACKSIZE;

	coefstack[top] = 1.0f;
	raystack[top].origin = camera->position;
	raystack[top].direction = normalize(camera->forward + camera->right * (x - camera->half_resolution.x) + camera->up * (y - camera->half_resolution.y));
	avoidstack[top] = -1;
	++top;

	do
	{
		Ray const	ray = raystack[--top];
		Impact		impact;

		if (find_closest(root, nodes, &ray, &impact, avoidstack[top]))
		{
			float4	accumulated_color = (float4)(0.25f, 0.25f, 0.25f, 0.25f);
			Ray		shadowray;

			shadowray.origin = ray.origin + ray.direction * impact.near;

			for (unsigned int i = 0; i < lights_count; ++i)
			{
				shadowray.direction = lights[i].position - shadowray.origin;
				float const	distance = length(shadowray.direction);
				shadowray.direction /= distance;

				//float const received_power = dot(ray.direction.xyz, impact.normal.xyz) * (lights[i].color.w / distance);
				float const received_power = dot(impact.normal, shadowray.direction);
				if (received_power > FLT_EPSILON && compute_shadow(root, nodes, &shadowray, distance, impact.index))
				{
					accumulated_color.xyz += lights[i].color.xyz * received_power;
					accumulated_color.w += received_power;
				}
			}

			int	const	mtlindex = nodes[impact.index].left;
			float const	coef = coefstack[top];

			color += materials[mtlindex].color * accumulated_color * (1.0f - materials[mtlindex].reflection) * coefstack[top];
		
			if ((coefstack[top] = coef * materials[mtlindex].reflection) > FLT_EPSILON && maxray > 0)
			{
				--maxray;
				raystack[top].origin = shadowray.origin;
				raystack[top].direction = ray.direction - impact.normal * dot(ray.direction, impact.normal) * 2.0f;
				coefstack[top] *= materials[mtlindex].reflection;
				avoidstack[top] = impact.index;
				++top;
			}
			/*
			if ((coefstack[top] = coef * materials[mtlindex].color.w) > FLT_EPSILON && top < RAYSTACKSIZE)
			{	
				float const	n = 1.0f / materials[mtlindex].refraction;
				float const c = -dot(impact.normal, ray.direction);
				float const s = 1.0 - n * n * (1.0f - c * c);
				if (s > 0.0f)
				{
					Ray	refractionray;
					refractionray.origin = shadowray.origin;
					refractionray.direction = ray.direction * n + impact.normal * (n * c - s);
					int	idx;
					refractionray.origin += refractionray.direction * find_closest_s(root, nodes, &refractionray, mtlindex, &idx);
					refractionray.direction = -refractionray.direction;
					find_closest(root, nodes, &refractionray, &impact, idx);
					
					float const	no = materials[mtlindex].refraction;
					float const co = -dot(impact.normal, -refractionray.direction);
					float const so = 1.0 - no * no * (1.0f - co * co);
					if (s > 0.0f)
					{
						raystack[top].origin = refractionray.origin + refractionray.direction * impact.near;
						raystack[top].direction = -refractionray.direction * no + -impact.normal * (no * co - so);
						avoidstack[top] = impact.index;
						++top;
					}
					//else inner reflection
				}
				//else outter reflection
			}*/
		}
	}
	while (top);

	//color += (float4)(0.2f, 1.0f, 1.0f, 1.0f);
	color *= 255.0f;
	write_imageui(image, (int2)(x, y), (uint4)(color.z, color.y, color.x, color.w));
}
