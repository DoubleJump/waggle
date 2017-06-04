

static b32
point_in_rect(Vec3 p, Vec4 r)
{
	return p.x >= r.x && p.x <= (r.x + r.z) &&
		   p.y <= r.y && p.y >= (r.y + r.w);
}

static b32
point_in_rect(Vec3 p, Vec4 r, Mat4 matrix)
{
	auto inv = inverse_affine(matrix);
	auto tp = mul_point(p, inv);
	return point_in_rect(tp, r);
}

static b32
ray_sphere(Hit_Info* info, Ray ray, Vec3 origin, f32 radius)
{
	info->hit = false;

	auto delta = ray.origin - origin;
	auto a = dot(ray.direction, ray.direction);
	auto b = dot(ray.direction, delta);
	auto c = dot(delta, delta) - (radius * radius);
	auto d = b * b - c * a;

	if(d < 0) return false;

	auto rd = sqrtf(d);
	a = 1 / a;

	auto t0 = (-b + rd) * a;
	auto t1 = (-b - rd) * a;

	if(t0 < t1) info->t = t0;
	else info->t = t1;

	info->hit = true;
	info->point = ray.origin + (ray.direction * info->t);
	info->normal = normalize(origin - info->point);
	return true;
}

static b32
circle_circle(Hit_Info* info, Vec3 pa, f32 ra, Vec3 pb, f32 rb)
{
	info->hit = false;
	auto delta = pb - pa;

	auto dist = distance(pa, pb);
	auto rad_sum = ra + rb;
	if(dist < rad_sum)
	{
		info->hit = true;
		info->t = rad_sum - dist;
		info->normal = normalize(delta);
		info->point = pa + (info->normal * ra);
		return true;
	}
	return false;
}

static b32
point_in_circle(Vec3 p, Vec3 c, f32 r)
{
	auto delta = c - p;
	auto l = sqr_length(delta);
	return (l < r * r);
}

static b32
line_circle(Hit_Info* info, Vec3 center, f32 radius, Vec3 la, Vec3 lb)
{
	auto da = la - center;
	auto db = lb - center;

	auto s = db - da;
	auto a = sqr_length(s);
	auto b = 2 * ((s.x * da.x) + (s.y * da.y));
	auto c = sqr_length(da) - (radius * radius);
	auto delta = b * b - (4 * a * c);

	info->hit = false;
	if(delta < 0) return false;

	auto sd = sqrtf(delta);
	auto ta = (-b - sd) / (2 * a);

	if(ta < 0 || ta > 1) return false;

	info->point = la * (1 - ta) + ta * lb;
	auto tb = (-b + sd) / (2 * a);

	if(abs(ta - 0.5f) < abs(tb - 0.5f))
	{
		info->hit = true;
		info->point = la * (1 - tb) + tb * lb;
		return true;
	}
	return false;
}

static b32
line_line(Hit_Info* info, Vec3 a, Vec3 b, Vec3 c, Vec3 d)
{
	auto la = b - a;
	auto lb = d - c;
	auto divisor = -lb.x * la.y + la.x * lb.y;

	auto ac = a-c;
	auto s = (-la.y * ac.x + la.x * ac.y) / divisor;
	auto t = ( lb.x * ac.y - lb.y * ac.x) / divisor;

	if(s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		info->hit = true;
		info->point = a + (lb * t);
		return true;
	}

	info->hit = false;
	return false;
}

static b32
aabb_aabb(AABB a, AABB b)
{
	if(a.min.x < b.max.x) return false;
	if(a.max.x < b.min.x) return false;
   	if(a.min.y > b.max.y) return false;
   	if(a.max.y < b.min.y) return false;
   	if(a.min.z > b.max.z) return false;
   	if(a.max.z < b.min.z) return false;
   	return true;
}

static b32
aabb_ray(Hit_Info* info, AABB a, Ray r)
{
	Vec3 f = 1.0f / r.direction;

	auto t1 = (a.min.x - r.origin.x) * f.x;
	auto t2 = (a.max.x - r.origin.x) * f.x;
	auto t3 = (a.min.y - r.origin.y) * f.y;
	auto t4 = (a.max.y - r.origin.y) * f.y;
	auto t5 = (a.min.z - r.origin.z) * f.z;
	auto t6 = (a.max.z - r.origin.z) * f.z;

	auto tmin = maximum(maximum(minimum(t1,t2), minimum(t3,t4)), minimum(t5,t6));
	auto tmax = minimum(minimum(maximum(t1,t2), maximum(t3,t4)), maximum(t5,t6));

	if(tmax < 0 || tmin > tmax)
	{
		info->hit = false;
		return false;
	}

	info->hit = true;
	info->t = tmin;
	info->point = r.origin + (r.direction * tmin);
	//info->normal;
	return true;
}

static b32
ray_plane(Hit_Info* info, Ray ray, Plane plane)
{
	info->hit = false;
	f32 d = dot(plane.normal, ray.direction);
	//NOTE: use abs(d) detect back face of plane

	f32 epsilon = 0.0001f;
	//if(d < -epsilon) 
	if(d > -epsilon) return false; 
	
	    f32 t = dot((plane.position - ray.origin), plane.normal) / d;
	    info->t = t;
	    info->normal = plane.normal;
	    info->point = ray.origin + (ray.direction * t);

	    if(t > -epsilon && t <= ray.length) 
	   // if(t > 0 && t <= ray.length + epsilon) 
	    {
	    	info->hit = true;
	    	return true;
	    }
	
	return false;
}

static Vec3
closest_point_on_line(Vec3 p, Vec3 a, Vec3 b)
{
	auto ap = p - a;       
    auto ab = b - a;      
    auto mag = sqr_length(ab);  
    auto d = dot(ap, ab); 
    auto dist = d / mag;

    if(dist < 0) return a;
    else if(dist > 1) return b;
    else return a + ab * dist;
}

static b32
point_in_triangle(Vec3 p, Triangle t)
{
	auto v0 = t.c - t.a;
	auto v1 = t.b - t.a;
	auto v2 = p - t.a;

	auto dot00 = dot(v0, v0);
	auto dot01 = dot(v0, v1);
	auto dot02 = dot(v0, v2);
	auto dot11 = dot(v1, v1);
	auto dot12 = dot(v1, v2);

	// Barycentric coordinates
	auto inv = 1.0 / (dot00 * dot11 - dot01 * dot01);
	auto u = (dot11 * dot02 - dot01 * dot12) * inv;
	auto v = (dot00 * dot12 - dot01 * dot02) * inv;

	return (u >= 0) && (v >= 0) && (u + v < 1);
}

static b32
point_in_triangle(Hit_Info* info, Vec3 p, Triangle t, f32 radius)
{
	Plane plane = plane_from_triangle(t);
	f32 rr = radius * radius;

	Vec3 ca = closest_point_on_line(p, t.a, t.b);
	Vec3 cb = closest_point_on_line(p, t.b, t.c);
	Vec3 cc = closest_point_on_line(p, t.c, t.a);
	
	f32 da = abs(sqr_length(p-ca));
	f32 db = abs(sqr_length(p-cb));
	f32 dc = abs(sqr_length(p-cc));

	if(da <= db && da <= dc)
	{
	    auto edge = t.b - t.a;      
	    auto dist = dot(p - t.a, edge) / sqr_length(edge);

	    if(dist < 0)
	    {
	    	info->normal = normalize(p - t.a);
	    	if(da < rr) return true;
	    }
	    else if(dist > 1) 
	    {
	    	info->normal = normalize(p - t.b);
	    	if(da < rr) return true;
	    }
	    else //edge
	    {
	    	Vec3 n = ortho_normalize(t.a-t.b, -plane.normal);
	    	auto center = (t.a + t.b) / 2;
			auto sd = dot(n, p - center);

			if(sd < radius)
			{
				sd = clamp(sd / radius, 0.0,1.0);
				info->normal = normalize(lerp(plane.normal, n,sd));
				return true;
			}
	    }
	}
	else if(db < da && db < dc)
	{
		auto edge = t.c - t.b;      
	    auto dist = dot(p - t.b, edge) / sqr_length(edge);

	    if(dist < 0) 
	    {
	    	info->normal = normalize(p - t.b);
	    	if(db < rr) return true;
	    }
	    else if(dist > 1) 
	    {
	    	info->normal = normalize(p - t.c);
	    	if(db < rr) return true;
	    }
	    else //edge
	    {
	    	Vec3 n = ortho_normalize(t.b-t.c, -plane.normal);
	    	auto center = (t.b + t.c) / 2;
			auto sd = dot(n, (p - center));

			if(sd < radius)
			{
				sd = clamp(sd / radius, 0.0,1.0);
				info->normal = normalize(lerp(plane.normal, n,sd));
				return true;
			}
	    }
	}
	else
	{
		auto edge = t.a - t.c;      
	    auto dist = dot(p - t.c, edge) / sqr_length(edge);

	    if(dist < 0) 
	    {
	    	info->normal = normalize(p - t.c);
	    	if(db < rr) return true;
	    }
	    else if(dist > 1) 
	    {
	    	info->normal = normalize(p - t.a);
	    	if(db < rr) return true;
	    }
	    else //edge
	    {
	    	Vec3 n = ortho_normalize(t.c-t.a, -plane.normal);
	    	auto center = (t.a + t.c) / 2;
			auto sd = dot(n, (p - center));

			if(sd < radius)
			{
				sd = clamp(sd / radius, 0.0,1.0);
				info->normal = normalize(lerp(plane.normal, n,sd));
				return true;
			}
	    }
	}

	return false;
}

static b32
ray_triangle(Hit_Info* info, Ray ray, Triangle tri)
{
	info->hit = false;

	auto e1 = tri.b - tri.a;
	auto e2 = tri.c - tri.a;

	auto h = cross(ray.direction, e2);
	auto a = dot(e1, h);

	//if (a > -EPSILON && a < EPSILON) return false;

	if (a > -EPSILON) return false;

	auto f = 1.0 / a;
	auto s = ray.origin - tri.a;
	auto u = f * (dot(s,h));

	if (u < 0.0 || u > 1.0) return false;

	auto q = cross(s,e1);
	auto v = f * dot(ray.direction,q);

	if (v < 0.0 || u + v > 1.0) return false;

	auto t = f * dot(e2, q);

	if(t > EPSILON) 
	{
		info->hit = true;
		info->t = t;
		info->point = ray.origin + (ray.direction * t);
		info->normal = get_normal(tri);
		return true;
	}

	// line intersection but not a ray intersection
	return false;
}


static b32
ray_mesh(Hit_Info* info, Ray ray, Mesh* mesh, Mat4* matrix = NULL)
{
	auto vb = &mesh->vertex_buffer;
	auto ib = &mesh->index_buffer;
	
	auto stride = vb->stride;
	auto vertex_data = vb->data;
 
	//TODO: transform ray and result with matrix
	if(matrix)
	{
		auto inv = -(*matrix);
		ray.origin = mul_point(ray.origin, inv);
		ray.direction = mul_dir(ray.direction, inv);
	}
	
	Hit_Info closest;
	closest.t = F32_MAX;
	closest.hit = false;

	if(mesh->use_index_buffer)
	{
		auto index_data = ib->data;
		auto n = ib->count;
		for(auto i = 0; i < n; i+=3)
		{
			auto ia = index_data[i  ] * stride;
			auto ib = index_data[i+1] * stride;
			auto ic = index_data[i+2] * stride;
			
			Triangle t;
			t.a = *(Vec3*)(vertex_data + ia);
			t.b = *(Vec3*)(vertex_data + ib);
			t.c = *(Vec3*)(vertex_data + ic);

			ray_triangle(info, ray, t);
			if(info->hit && info->t < closest.t) closest = *info;
		} 
	}
	else
	{
		auto n = vb->count;
		for(auto i = 0; i < n; i+=3)
		{
			auto ia = (i  ) * stride;
			auto ib = (i+1) * stride;
			auto ic = (i+2) * stride;

			Triangle t;
			t.a = *(Vec3*)(vertex_data + ia);
			t.b = *(Vec3*)(vertex_data + ib);
			t.c = *(Vec3*)(vertex_data + ic);
			
			ray_triangle(info, ray, t);
			if(info->hit && info->t < closest.t) closest = *info;
		}
	}

	if(matrix)
	{
		closest.point = mul_point(closest.point, *matrix);
		closest.normal = mul_dir(closest.normal, *matrix);
	}

	*info = closest;
	return info->hit; 
}


static Vec3
capsule_sweep_triangle(Hit_Info* info, Capsule c, Vec3 velocity, Triangle tri)
{
	info->hit = false;
	Vec3 response = {0,0,0};

	f32 hh = c.height / 2;
	Vec3 top = c.position;
	top.y += hh;
	Vec3 btm = c.position;
	btm.y -= hh;
	Vec3 pA, pB;

	// TODO: pass these in
	f32 mag = length(velocity);
	if(mag < EPSILON) return response;

	Vec3 dir = normalize(velocity);
	auto plane = plane_from_triangle(tri);
	plane.position += plane.normal * c.radius;

	b32 collision = false;

	Ray ray = {top, dir, mag};
	if(ray_plane(info, ray, plane)) 
	{
		pA = info->point;
		collision = true;
	}
	
	ray.origin = btm;
	if(ray_plane(info, ray, plane))
	{
		pB = info->point;
		collision = true;
	}

	if(!collision) 
	{
		info->hit = false;
		return response;
	}

	if(!point_in_triangle(info, pA, tri, c.radius))
	{
		if(!point_in_triangle(info, pB, tri, c.radius))
		{
			info->hit = false;
			return response;
		}
	}

	//TODO: modify the normal depending on distance

	//ctx->color = col_aqua;
	//draw_wire_triangle(ctx, tri);

	Vec3 center = (pA + pB) / 2.0;
	info->hit = true;
	info->point = center;
	info->normal = plane.normal;

	return center + (info->normal * 0.001);
	//return center + (info->normal);
}

static Vec3
capsule_sweep_mesh(Hit_Info* info, Capsule c, Vec3 velocity, Mesh* mesh)
{
	auto vb = &mesh->vertex_buffer;
	auto stride = vb->stride;
	auto vertex_data = vb->data;

	//TODO: transform inout and result with matrix
	Vec3 result;

	if(mesh->use_index_buffer)
	{
		auto ib = &mesh->index_buffer;
		auto index_data = ib->data;
		auto n = ib->count;
		for(auto i = 0; i < n; i+=3)
		{
			auto ia = index_data[i  ] * stride;
			auto ib = index_data[i+1] * stride;
			auto ic = index_data[i+2] * stride;
			
			Triangle t;
			t.a = *(Vec3*)(vertex_data + ia);
			t.b = *(Vec3*)(vertex_data + ib);
			t.c = *(Vec3*)(vertex_data + ic);

			Vec3 response = capsule_sweep_triangle(info, c, velocity, t);

			if(info->hit)
			{
				result = response;
				break;
			}
		}
	}
	else
	{
		auto n = vb->count;
		for(auto i = 0; i < n; i+=3)
		{
			auto ia = (i  ) * stride;
			auto ib = (i+1) * stride;
			auto ic = (i+2) * stride;

			Triangle t;
			t.a = *(Vec3*)(vertex_data + ia);
			t.b = *(Vec3*)(vertex_data + ib);
			t.c = *(Vec3*)(vertex_data + ic);
			
			Vec3 response = capsule_sweep_triangle(info, c, velocity, t);

			if(info->hit)
			{
				result = response;
				break;
			}
		}
	}
	
	return result; 
}