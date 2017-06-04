struct Rigidbody
{
	f32 mass;
	f32 inv_mass;
	Vec3 position;
	Vec3 last_position;
	Vec3 accleration;
	Vec3 velocity;
	Vec3 angular_acceleration;
	Vec3 angular_velocity;
	f32 restitution;
};

struct Distance_Constraint
{
	Rigidbody* a;
	Rigidbody* b;
	f32 stiffness;
	f32 min_distance;
	f32 max_distance;
};

static void
update_constraint(Distance_Constraint* c)
{
	auto delta = c->a->position - c->b->position;
	auto d = length(delta);

	if(d < c.max_distance && d > c.min_distance) return;
	if(d > c.max_distance) d = (c.max_distance - d) / d;
	else d = (c.min_distance - d) / d;

	auto ma = c->a->inv_mass;
	auto mb = c->b->inv_mass;
	auto fa = ((ma / (mb + ma)) * c->stiffness) * d;
	auto fb = (c->stiffness - fa) * d;

	c->a->position += delta * fa;
	c->b->position -= delta * fb;
}

static void
verlet_integration(Rigidbody* b, f32 dt)
{
	auto dts = dt * dt;
	auto velocity = (b->position - b->last_position) * 0.95;

	body->last_position = b->position;
	body->position = b->position + velocity * 0.5f * b->acceleration * dts;
	body->acceleration = {0,0,0};
}

static void
euler_integration(Rigidbody* b, f32 dt)
{
	b->velocity += b->acceleration * b->inv_mass * dt;
	b->position += b->velocity;
	b->acceleration = {0,0,0};
	b->velocity *= 0.9f;
}

/*
integrate_RK4: function(state, t, dt)
{
	var _t = gb.physics;

	// -- _t.evaluate(_t.da, state, t);
	_t.evaluate(_t.db, state, t, dt * 0.5, _t.a);
	_t.evaluate(_t.dc, state, t, dt * 0.5, _t.b);
	_t.evaluate(_t.dd, state, t, dt, _t.c);
	
	var i = 1.0 / 6.0;
	state.position += i * dt * (_t.da.velocity + 2.0 * (_t.db.velocity + _t.dc.velocity) + _t.dd.velocity);
	state.momentum += i * dt * (_t.da.force + 2.0 * (_t.db.force + _t.dc.force) + _t.dd.force);
	state.orientation += i * dt * (_t.da.spin + 2.0 * (_t.db.spin + _t.dc.spin) + _t.dd.spin);
	state.ang_momentum += i * dt * (_t.da.torque + 2.0 *(_t.db.torque + _t.dc.torque) + _t.dd.torque);

	_t.recalculate_state(state);
}
*/

static void
apply_impulse(Rigidbody* b, Vec3 v)
{
	b->velocity += v * b->inv_mass;
}

static void 
apply_force(Rigidbody* b, Vec3 v)
{
	b->acceleration += v * b->inv_mass;
}