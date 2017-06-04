static Plane
plane_from_triangle(Triangle t)
{
	Plane p;
	p.position = get_center(t);
	p.normal = get_normal(t);
	return p;
}

static f32 
signed_distance_to_plane(Vec3 point, Plane plane)
{
    return dot(plane.normal, (point - plane.position));
}

static Vec3
project_onto_plane(Vec3 point, Plane plane)
{
    auto distance = -signed_distance_to_plane(point, plane);
    return point + (plane.normal * distance);
}