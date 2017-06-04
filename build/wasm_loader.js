var Module = {};

function init()
{
	var width = document.body.clientWidth;
	var height = document.body.clientHeight;
	var canvas = document.createElement("canvas");
	canvas.setAttribute('id', 'app');
	canvas.width = width;
	canvas.height = height;
	document.body.appendChild(canvas);
	
	Module.canvas = canvas;

	if(window['WebAssembly'] === undefined)
	{
		var script = document.createElement('script');
		script.src = "app_asm.js";
		document.body.appendChild(script);
	}
	else
	{
		request_wasm_file('app_wasm.wasm', Module);
	}
}

function request_wasm_file(url, module)
{
	var r = new XMLHttpRequest();
	r.open('GET', url, true);
	r.responseType = 'arraybuffer';
	r.onload = function() 
	{
		module.wasmBinary = r.response;
		var script = document.createElement('script');
		script.src = "app_wasm.js";
		document.body.appendChild(script);
	};
	r.send();
}

init();