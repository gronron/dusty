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

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
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

#version 450 core

#define MAXRAY 4
#define RAYSTACKSIZE 2 //should be as low as possible
#define SEARCHSTACKSIZE 24 //should be as low as possible

const float INFINITY = 1.0f / 0.0f;
const float FLT_EPSILON = 0.0001f;

///////////////////////////////////////////////////////////////////////////////

struct		AabbNode
{
	vec3	bottom;
	int		children;
	vec3	top;
	int		matrix;
};

struct		Camera
{
	vec4	position;
	vec4	forward;
	vec4	right;
	vec4	up;
	vec2	half_resolution;
};

struct		Light
{
	vec4	position;
	vec4	color;
};

struct		Material
{
	vec4	color;
	float	refraction;
	float	reflection;
	float	shining;
};

///////////////////////////////////////////////////////////////////////////////

struct		Aabb
{
	vec3	bottom;
	vec3	top;
};

struct		Ray
{
	vec3	origin;
	vec3	direction;
};

struct		Impact
{
	vec3	normal;
	float	near;
	float	far;
	int		index;
};

///////////////////////////////////////////////////////////////////////////////

layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
out vec4 color;

layout (std140) uniform _camera
{
    Camera camera;
};

layout (std430) readonly buffer _nodes
{
    AabbNode nodes[];
};

layout (std430) readonly buffer _matrices
{
    mat3 matrices[];
};

layout (std430) readonly buffer _materials
{
    Material materials[];
};

uniform uint lights_number;

layout (std430) readonly buffer _lights
{
    Light lights[];
};

///////////////////////////////////////////////////////////////////////////////

int	stack[SEARCHSTACKSIZE];

///////////////////////////////////////////////////////////////////////////////

bool			intersect_rayaabb(in const vec3 origin, in const vec3 direction, in const int aabbidx, out float tnear, out float tfar)
{
	const vec3	tbot = (nodes[aabbidx].bottom - origin) * direction;
	const vec3	ttop = (nodes[aabbidx].top - origin) * direction;
	const vec3	tmin = min(tbot, ttop);
	const vec3 	tmax = max(tbot, ttop);

	tnear = max(max(tmin.x, tmin.y), tmin.z);
	tfar = min(min(tmax.x, tmax.y), tmax.z);

	return (tfar > 0.0f && tnear <= tfar);
}

bool			intersect_rotated_rayaabb(in const vec3 origin, in const vec3 direction, in const int aabbidx, out float tnear, out float tfar)
{
	const mat3  rotation = matrices[nodes[aabbidx].matrix];
	const vec3  center = (nodes[aabbidx].bottom + nodes[aabbidx].top) * 0.5f;
	const vec3	rori = (origin - center) * rotation + center;
	const vec3	rdir = direction * transpose(rotation);
	const vec3	tbot = (nodes[aabbidx].bottom - rori) * rdir;
	const vec3	ttop = (nodes[aabbidx].top - rori) * rdir;
	const vec3	tmin = min(tbot, ttop);
	const vec3 	tmax = max(tbot, ttop);

	tnear = max(max(tmin.x, tmin.y), tmin.z);
	tfar = min(min(tmax.x, tmax.y), tmax.z);

	return (tfar > 0.0f && tnear <= tfar);
}

bool			compute_rotated_rayaabb_normal(in const vec3 origin, in const vec3 direction, in const int aabbidx, out vec3 normal)
{
	const mat3  rotation = matrices[nodes[aabbidx].matrix];
	const vec3  center = (nodes[aabbidx].bottom + nodes[aabbidx].top) * 0.5f;
	const vec3	rori = (origin - center) * rotation + center;
	const vec3	rdir = direction * transpose(rotation);
	const vec3	tbot = (nodes[aabbidx].bottom - rori) * rdir;
	const vec3	ttop = (nodes[aabbidx].top - rori) * rdir;
	const vec3 	tmin = min(tbot, ttop);

	if (tmin.x > tmin.y)
	{
		if (tmin.x > tmin.z)
			normal = vec3(rdir.x >= 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f);
		else
			normal = vec3(0.0f, 0.0f, rdir.z >= 0.0f ? -1.0f : 1.0f);
	}
	else
	{
		if (tmin.y > tmin.z)
			normal = vec3(0.0f, rdir.y >= 0.0f ? -1.0f : 1.0f, 0.0f);
		else
			normal = vec3(0.0f, 0.0f, rdir.z >= 0.0f ? -1.0f : 1.0f);
	}
	normal = normal * rotation;
	return (true);
}

bool			find_closest(in const vec3 origin, in const vec3 direction, inout Impact data)
{
	const vec3	invdirection = 1.0f / direction;
	const int	idx = data.index;
	int			top = 1;
	int			index = 0;

	stack[0] = -1;
	do
	{
		float	lnear;
		float	rnear;
		float	far;

		const int	left = nodes[index].children;
		const int	right = nodes[index].children + 1;
		bool		ltrue = intersect_rayaabb(origin, invdirection, left, lnear, far) && lnear < data.near;
		bool		rtrue = intersect_rayaabb(origin, invdirection, right, rnear, far) && rnear < data.near;

		if (nodes[left].children < 0)
		{
			if (intersect_rotated_rayaabb(origin, invdirection, left, lnear, far) && left != idx && lnear < data.near)
			{
				data.index = left;
				data.near = lnear;
			}
			ltrue = false;
		}

		if (nodes[right].children < 0)
		{
			if (intersect_rotated_rayaabb(origin, invdirection, right, rnear, far) && right != idx && rnear < data.near)
			{
				data.index = right;
				data.near = rnear;
			}
			rtrue = false;
		}

		if (ltrue && rtrue)
		{
			if (lnear > rnear)
			{
				index = right;
				stack[top++] = left;
			}
			else
			{
				index = left;
				stack[top++] = right;
			}
		}
		else if (ltrue)
			index = left;
		else if (rtrue)
			index = right;
		else
			index = stack[--top];
	}
	while (index != -1);

	return (data.index != -1 ? compute_rotated_rayaabb_normal(origin, invdirection, data.index, data.normal) : false);
}

float			find_closest_s(in const vec3 origin, in const vec3 direction, in const int value, out int idx)
{
	const vec3	invdirection = 1.0f / direction;
	int			top = 0;
	float		near = INFINITY;

	idx = -1;
	stack[top] = 0;
	do
	{
		const int	index = stack[top];

		float	lnear;
		float	rnear;
		float	far;

		const int	left = nodes[index].children;
		const int	right = nodes[index].children + 1;
		bool		ltrue = (0 - nodes[left].children) == value ? false : (intersect_rayaabb(origin, invdirection, left, lnear, far) && lnear < near);
		bool		rtrue = (0 - nodes[right].children) == value ? false : (intersect_rayaabb(origin, invdirection, right, rnear, far) && rnear < near);

		if (nodes[left].children < 0)
		{
			if ((0 - nodes[left].children) != value && intersect_rotated_rayaabb(origin, invdirection, left, lnear, far) && lnear < near)
			{
				idx = left;
				near = lnear;
			}
			ltrue = false;
		}

		if (nodes[right].children < 0)
		{
			if ((0 - nodes[right].children) != value && intersect_rotated_rayaabb(origin, invdirection, right, rnear, far) && rnear < near)
			{
				idx = right;
				near = rnear;
			}
			rtrue = false;
		}

		if (lnear > rnear)
		{
			if (ltrue)
				stack[top++] = left;
			if (rtrue)
				stack[top++] = right;
		}
		else
		{
			if (rtrue)
				stack[top++] = right;
			if (ltrue)
				stack[top++] = left;
		}
	}
	while (--top >= 0);

	if (idx == -1)
	{
		float	far;
		intersect_rotated_rayaabb(origin, invdirection, 0, far, near);
	}
	return (near);
}

bool			compute_shadow(in const vec3 origin, in const vec3 direction, in const float distance, in const int idx, inout vec4 color)
{
	const vec3	invdirection = 1.0f / direction;
	int			top = 0;

	stack[top] = 0;
	do
	{
		const int	index = stack[top];

		float	lnear;
		float	rnear;
		float	lfar;
		float	rfar;

		const int	left = nodes[index].children;
		const int	right = nodes[index].children + 1;
		bool		ltrue = intersect_rayaabb(origin, invdirection, left, lnear, lfar) && lnear < distance;
		bool		rtrue = intersect_rayaabb(origin, invdirection, right, rnear, rfar) && rnear < distance;

		if (nodes[left].children < 0)
		{
			if (intersect_rotated_rayaabb(origin, invdirection, left, lnear, lfar) && left != idx)
			{
				const int	mtlindex = 0 - nodes[left].children;
				if (materials[mtlindex].color.w != 0.0f)
				{
					const float coef = materials[mtlindex].color.w * (1.0f - (lfar - lnear));
					color *= coef + (1.0f - coef) * materials[mtlindex].color;
				}
				else
					return (false);
			}
			ltrue = false;
		}

		if (nodes[right].children < 0)
		{
			if (intersect_rotated_rayaabb(origin, invdirection, right, rnear, rfar) && right != idx)
			{
				const int	mtlindex = 0 - nodes[right].children;
				if (materials[mtlindex].color.w != 0.0f)
				{
					const float coef = materials[mtlindex].color.w * (1.0f - (rfar - rnear));
					color *= coef + (1.0f - coef) * materials[mtlindex].color;
				}
				else
					return (false);
			}
			rtrue = false;
		}

		if (ltrue)
			stack[top++] = left;
		if (rtrue)
			stack[top++] = right;
	}
	while (--top >= 0);

	return (true);
}

float		compute_aocclusion(in const vec3 origin, in const vec3 normal)
{
	Aabb	aabb;
	float	aocclusion = 8.0f;
	int		top = 0;

	aabb.bottom = (origin - vec3(1.0f, 1.0f, 1.0f)) + normal * (1.0f + FLT_EPSILON);
	aabb.top = aabb.bottom + vec3(2.0f, 2.0f, 2.0f);

	stack[top] = 0;
	do
	{
		const int	index = stack[top];

		const int	left = nodes[index].children;
		const int	right = nodes[index].children + 1;

		bool 		ltrue = nodes[left].bottom.x < aabb.top.x && nodes[left].bottom.y < aabb.top.y && nodes[left].bottom.z < aabb.top.z &&
							nodes[left].top.x > aabb.bottom.x && nodes[left].top.y > aabb.bottom.y && nodes[left].top.z > aabb.bottom.z;
		bool 		rtrue = nodes[right].bottom.x < aabb.top.x && nodes[right].bottom.y < aabb.top.y && nodes[right].bottom.z < aabb.top.z &&
							nodes[right].top.x > aabb.bottom.x && nodes[right].top.y > aabb.bottom.y && nodes[right].top.z > aabb.bottom.z;

		if (ltrue && nodes[left].children < 0)
		{
			const vec3	a = min(nodes[left].top, aabb.top) - max(nodes[left].bottom, aabb.bottom);
			aocclusion -= a.x * a.y * a.z * (1.0f - materials[0 - nodes[left].children].color.w);
			ltrue = false;
		}
		if (rtrue && nodes[right].children < 0)
		{
			const vec3	a = min(nodes[right].top, aabb.top) - max(nodes[right].bottom, aabb.bottom);
			aocclusion -= a.x * a.y * a.z * (1.0f - materials[0 - nodes[right].children].color.w);
			rtrue = false;
		}

		if (ltrue)
			stack[top++] = left;
		if (rtrue)
			stack[top++] = right;
	}
	while (--top >= 0);

	return (max(aocclusion * 0.125f, 0.0f) * 0.5f + 0.5f);
}

void		main()
{
	Ray		raystack[RAYSTACKSIZE];
	float	coefstack[RAYSTACKSIZE];
	int		avoidstack[RAYSTACKSIZE];
	int		top = 0;
	int		maxray = MAXRAY;

	color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	coefstack[top] = 1.0f;
	raystack[top].origin = camera.position.xyz;
	raystack[top].direction = normalize(camera.forward + camera.right * (gl_FragCoord.x - camera.half_resolution.x) + camera.up * (gl_FragCoord.y - camera.half_resolution.y)).xyz;
	avoidstack[top] = -1;

	do
	{
		vec3	origin = raystack[top].origin;
		vec3	direction = raystack[top].direction;
		Impact	impact;

		impact.index = avoidstack[top];
		impact.near = INFINITY;
		if (find_closest(origin.xyz, direction.xyz, impact))
		{
			vec4	accumulated_color = vec4(0.4f, 0.4f, 0.5f, 0.25f);

			origin += direction * impact.near;

			for (uint i = 0; i < lights_number; ++i)
			{
				vec3 ligth_direction = lights[i].position.xyz - origin;
				const float	distance = length(ligth_direction);
				const float inv_distance = 1.0f / distance;
				ligth_direction *= inv_distance;

				const float	received_power = dot(impact.normal, ligth_direction) * (lights[i].color.w * inv_distance);
				vec4		tcolor = lights[i].color;
				if (received_power > FLT_EPSILON && compute_shadow(origin.xyz, ligth_direction.xyz, distance, impact.index, tcolor))
				{
					accumulated_color.xyz += tcolor.xyz * received_power;
					accumulated_color.w += received_power;
				}
			}

			const float aocclusion = compute_aocclusion(origin, impact.normal);
			const int	mtlindex = 0 - nodes[impact.index].children;
			const float	coef = coefstack[top];

			color += materials[mtlindex].color * accumulated_color * (1.0f - materials[mtlindex].reflection) * (1.0f - materials[mtlindex].color.w) * aocclusion * coef;

			if ((coefstack[top] = coef * materials[mtlindex].reflection) > FLT_EPSILON && maxray != 0)
			{
				--maxray;
				raystack[top].origin = origin;
				raystack[top].direction = direction - impact.normal * dot(direction, impact.normal) * 2.0f;
				avoidstack[top++] = impact.index;
			}

			if ((coefstack[top] = coef * materials[mtlindex].color.w) > FLT_EPSILON && maxray != 0)
			{
				const float	n = 1.0f / materials[mtlindex].refraction;
				const float c = -dot(impact.normal, direction);
				const float s = 1.0f - n * n * (1.0f - c * c);

				if (s > 0.0f)
				{
					direction = direction * n + impact.normal * (n * c - s);
					origin += direction * find_closest_s(origin.xyz, direction.xyz, mtlindex, impact.index);
					direction = -direction;
					impact.near = INFINITY;
					find_closest(origin.xyz, direction.xyz, impact);

					const float	no = materials[mtlindex].refraction;
					const float co = -dot(impact.normal, -direction);
					const float so = 1.0f - no * no * (1.0f - co * co);
					if (s > 0.0f)
					{
						--maxray;
						raystack[top].origin = origin + direction * impact.near;
						raystack[top].direction = -direction * no + -impact.normal * (no * co - so);
						avoidstack[top++] = impact.index;
					}
					//else inner reflection
				}
				else
				{
					--maxray;
					raystack[top].origin = origin;
					raystack[top].direction = direction - impact.normal * dot(direction, impact.normal) * 2.0f;
					avoidstack[top++] = impact.index;
				}
			}
		}
	}
	while (--top >= 0);
	//float	l = dot(color.xyz, vec3(0.2126f, 0.7152f, 0.0722f));
	//l = l / (l + 0.1f);
	//color *= l;
	color.w = 1.0f;
}
