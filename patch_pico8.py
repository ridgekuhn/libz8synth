import argparse
from patcherex2 import InsertDataPatch, InsertFunctionPatch, ModifyFunctionPatch, Patcherex
from pathlib import Path
import os
import re

################
# Parse CLI Args
################
parser = argparse.ArgumentParser(
    prog="libp8synth patcher"
)

parser.add_argument("p8path", help="path to pico8 executable")

parser.add_argument(
	"-o, ", "--out", help="output path. default=\"./out/pico8_patched\"", default="./out/pico8_patched")

parser.add_argument(
	"-s", "--synth", help="synth mixers to use for patching. default=\"aliased\"", default="aliased")

args = parser.parse_args()

############
# Pre-flight
############
os.makedirs("./out", exist_ok=True)

hq_phasors = [
    ["phasor_hq_pulse", "./src/binary/audio/synth/phasors/hq/phasor_hq_pulse.c"],
    ["phasor_hq_sawtooth", "./src/binary/audio/synth/phasors/hq/phasor_hq_sawtooth.c"],
    ["phasor_hq_square", "./src/binary/audio/synth/phasors/hq/phasor_hq_square.c"],
    ["phasor_hq_tilted", "./src/binary/audio/synth/phasors/hq/phasor_hq_tilted.c"],
    ["phasor_hq_triangle", "./src/binary/audio/synth/phasors/hq/phasor_hq_triangle.c"],
   	# must come after phasor_hq_square, phasor_hq_triangle
    ["phasor_hq_organ", "./src/binary/audio/synth/phasors/hq/phasor_hq_organ.c"],
]

aliased_oscillators = [
    ["osc_aliased_brown_noise",
    	"./src/binary/audio/synth/oscillators/aliased/osc_aliased_brown_noise.c"],
    ["osc_aliased_pink_noise",
    	"./src/binary/audio/synth/oscillators/aliased/osc_aliased_pink_noise.c"],
    ["osc_aliased_white_noise",
    	"./src/binary/audio/synth/oscillators/aliased/osc_aliased_white_noise.c"],
   	# must come after other noise oscillators
    ["osc_aliased_noise",
    	"./src/binary/audio/synth/oscillators/aliased/osc_aliased_noise.c"],
    ["osc_aliased_wavetable",
    	"./src/binary/audio/synth/oscillators/aliased/osc_aliased_wavetable.c"],
]

hq_oscillators = [
    ["osc_hq_organ", "./src/binary/audio/synth/oscillators/hq/osc_hq_organ.c"],
    ["osc_hq_pulse", "./src/binary/audio/synth/oscillators/hq/osc_hq_pulse.c"],
    ["osc_hq_sawtooth", "./src/binary/audio/synth/oscillators/hq/osc_hq_sawtooth.c"],
    ["osc_hq_square", "./src/binary/audio/synth/oscillators/hq/osc_hq_square.c"],
    ["osc_hq_triangle", "./src/binary/audio/synth/oscillators/hq/osc_hq_triangle.c"],
    ["osc_hq_tilted", "./src/binary/audio/synth/oscillators/hq/osc_hq_tilted.c"],
]

#######
# Patch
#######
patcher = Patcherex(args.p8path)

# patcher.patches.append(InsertFunctionPatch(
# 	"polyblep",
# 	Path("./src/binary/audio/synth/filters/polyblep.c").read_text(),
# 	compile_opts={"extra_compiler_flags": [
# 		"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/audio/synth/filters", "-v"]}
# ))

for phasor in hq_phasors:
	patcher.patches.append(InsertFunctionPatch(
		phasor[0],
		Path(phasor[1]).read_text(),
		compile_opts={"extra_compiler_flags": ["-I", os.path.dirname(
			os.path.realpath(__file__)) + "/src/binary/audio/synth/phasors/hq", "-v"]}
	))

for oscillator in aliased_oscillators:
	code = Path(oscillator[1]).read_text()
	code = re.sub("#include .*codo_random\\.h\"", "", code)

	patcher.patches.append(InsertFunctionPatch(
		oscillator[0],
		code,
		compile_opts={"extra_compiler_flags": ["-I", os.path.dirname(
			os.path.realpath(__file__)) + "/src/binary/audio/synth/oscillators/aliased", "-v"]}
	))

for oscillator in hq_oscillators:
	patcher.patches.append(InsertFunctionPatch(
		oscillator[0],
		Path(oscillator[1]).read_text(),
		compile_opts={"extra_compiler_flags": ["-I", os.path.dirname(
			os.path.realpath(__file__)) + "/src/binary/audio/synth/oscillators/hq", "-v"]}
	))

patcher.patches.append(InsertFunctionPatch(
	"mix_reverb",
	Path("./src/binary/audio/synth/mix_reverb.c").read_text(),
	compile_opts={"extra_compiler_flags": [
		"-I", os.path.dirname(os.path.realpath(__file__)) +
            "/src/binary/audio/synth",
		"-v"
	]}
))

patcher.patches.append(ModifyFunctionPatch(
	"mix_osc_tick_new",
	Path("./src/binary/audio/synth/mix_osc_tick.c").read_text(),
	compile_opts={"extra_compiler_flags": [
		"-I", os.path.dirname(os.path.realpath(__file__)) + "/src/binary/audio/synth", "-v"]}
))

patcher.apply_patches()

patcher.save_binary(args.out)
