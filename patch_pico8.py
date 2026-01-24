import argparse
from patcherex2 import InsertFunctionPatch, ModifyFunctionPatch, Patcherex
from pathlib import Path
import os

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
	"-s", "--synth", help="synth oscillators to use for patching. default=\"aliased\"", default="aliased")

args = parser.parse_args()

############
# Pre-flight
############
os.makedirs("./out", exist_ok=True)

oscillators = [
	["mix_organ", "./src/binary/oscillators/aliased/mix_organ.c"],
	["mix_pulse", "./src/binary/oscillators/aliased/mix_pulse.c"],
	["mix_sawtooth", "./src/binary/oscillators/aliased/mix_sawtooth.c"],
	["mix_supersaw", "./src/binary/oscillators/aliased/mix_supersaw.c"],
	["mix_triangle", "./src/binary/oscillators/aliased/mix_triangle.c"],
	["mix_wavetable", "./src/binary/oscillators/aliased/mix_wavetable.c"],
	["mix_brown_noise", "./src/binary/oscillators/aliased/mix_brown_noise.c"],
	["mix_pink_noise", "./src/binary/oscillators/aliased/mix_pink_noise.c"],
	["mix_white_noise", "./src/binary/oscillators/aliased/mix_white_noise.c"],
	# mix_noise must come after other noise oscillators
	["mix_noise", "./src/binary/oscillators/aliased/mix_noise.c"],
]

#######
# Patch
#######
p = Patcherex(args.p8path)

for o in oscillators:
	p.patches.append(InsertFunctionPatch(o[0], Path(o[1]).read_text()))

p.patches.append(InsertFunctionPatch(
    "mix_reverb", Path("./src/binary/mix_reverb.c").read_text()))

p.patches.append(ModifyFunctionPatch("mix_osc_tick_new",
                                     Path("./src/binary/mix_osc_tick.c").read_text()))

p.apply_patches()

p.save_binary(args.out)
