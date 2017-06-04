static void
init_time(Time* t, f64 now)
{
	t->now = now;
	t->last = now;
	t->elapsed = 0;
	t->scale = 1;
}

static void
update_time(Time* t, f64 now, b32 tick)
{
	t->now = now;
	t->dt = (now - t->last) / 1000.0f;
	t->last = now;
	if(tick) t->elapsed += t->dt;
}

static void
ping_pong(Cycler* c, f64 dt)
{
	c->value += dt * c->rate * c->direction;
	if(c->value > 1.0f || c->value < 0.0f)
	{
		c->direction *= -1.0f;
	}
}