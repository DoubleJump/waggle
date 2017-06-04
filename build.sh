#!/bin/bash
#build.sh

clear

#Remember we added this to the path permanently
#source /home/gareth/Documents/dev/emscripten/emsdk/emsdk_env.sh

INPUT=src/main.cpp
WASM_OUTPUT=build/app_wasm.js
ASM_OUTPUT=build/app_asm.js
OPTIMISATION=-O2
MEMORY_SIZE=128*1024*1024

python asset_compiler.py --debug=True --log=1
if emcc $INPUT $OPTIMISATION --std=c++11 -s WASM=1 -s TOTAL_MEMORY=$MEMORY_SIZE -o $WASM_OUTPUT; then
echo "AWWW YEAH!";
	
	#if emcc $INPUT $OPTIMISATION --std=c++11 -s WASM=0 -s TOTAL_MEMORY=$MEMORY_SIZE -o $ASM_OUTPUT; then
	#echo "ASM!";
	#fi

xdotool search --name "Mozilla Firefox" windowactivate --sync
xdotool search --name "Mozilla Firefox" key F5
else
echo "OH NO!";
fi

