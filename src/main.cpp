#include "gblib_dependencies.cpp"
#include "gblib.cpp"
#include "app.cpp"

/*
TODO:
----------------
- pvr
- vaos
- dds image auto convert
- multi project setup
- audio, mp3 loader
*/

static void
main_loop()
{
	b32 tick = engine->has_focus && engine->can_update;
	update_time(&engine->time, emscripten_get_now(), tick);

	if(!tick) return;
	update();
	render();
	
	update(&engine->input);   
}

static void
preload()
{
	if(engine->assets.loaded)
	{
		emscripten_cancel_main_loop();
		initialize();
		emscripten_set_main_loop(main_loop, 0, 0);
	}
}

int main(int argc, char** argv) 
{
	init_engine();
	load_asset_file("assets.bin", &engine->assets);
	emscripten_set_main_loop(preload, 0, 0);
	return 1;
}