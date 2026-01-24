# Pico-8 Host Application Memory

## Notice

* This document is incomplete and may contain inaccuracies.

* In web exports, memory addressing is optimized and unused memory bytes are not accounted for. As a result, various pointer addresses are offset differently than in the listing below, with increasingly larger discrepancies as addresses increase.

## Table of Contents

* [LOADED CART ROM](#loaded-cart-rom)

* [MIXER CHANNEL STATES](#mixer-channel-states)

* [NOTE LOOKUP TABLE](#note-lookup-table)

## LOADED CART ROM

The host representation of the cart ROM running on the guest. This representation is not 1:1 with the guest ROM/RAM state.

```
+0x0020: SFX
+0xae20: MUSIC PATTERNS
```

### SFX

```
0xaa00 total, 0x02a8 each

+0x0000: filter byte
+0x0004: editor mode
+0x0008: spd
+0x000c: loop start
+0x0010: loop end
+0x0014: STEP DATA
```

#### STEP DATA

```
+0x0000: pitch
+0x0004: waveform
+0x0008: vol
+0x000c: effect cmd
+0x0010: 1 if step uses meta instrument
```

### MUSIC PATTERNS

```
0x0100 total, 0x0004 each

+0x0000: ch 0 sfx idx
+0x0001: ch 1 sfx idx
+0x0002: ch 2 sfx idx
+0x0003: ch 3 sfx idx
```

---

## MIXER CHANNEL STATES

The mixer state accommodates 16 channels, but only channels `8` - `11` are used, for VM channels `0` - `3`.

The oscillator state and mixing functions read address offsets +`0x2020` and above, with the tick buffer being stored somewhere below, possibly along with the current channel state which has already been mixed. Addresses `0x2ee0` and above refer to an unknown grouping.

```
0x00037000 total, 0x3700 each

+0x2020: pointer to LOADED CART ROM
+0x2028: STEP STATE
+0x21ae: TICK BUFFER
+0x2d20: PATTERN STATE
+0x2d30: OSCILLATOR STATE
+0x2e90: ?
+0x2ee0: ? (0 if OSCILLATOR STATE step does not use meta instrument)
+0x2ee8: ?
+0x2eec: pattern ticks remaining
```

### STEP STATE

```
CHANNEL STATE + 0x2028

+0x0000: pointer to step sfx addr in LOADED CART ROM
+0x0008: current sfx tick
+0x000c: current step tick
+0x0010: spd
+0x0014: ch target & 0x3
+0x0028: pitch num
+0x0030: waveform
+0x0038: vol
+0x0040: effect cmd
```

### PATTERN STATE

```
CHANNEL STATE + 0x2d20

+0x0000: current pattern tick
+0x0004: chunk_buffer samples remaining
+0x000c: current pattern idx
```

### OSCILLATOR STATES

```
CHANNEL STATE + 0x2d30

0x???? total, 0x0140(?) ea

+0x0000: waveform 0-7, or 8 if wavetable bit set
+0x0004: current phase
+0x0008: phase increment (freq << 16 / 22050)
+0x000c: current detune phase
+0x0010: detune phase increment (same as phase increment if no detune)
+0x0014: ?
+0x0018: base sfx step pitch (sliding from)
+0x001c: target vol (sfx step vol << 8) (post-fx)
+0x0020: target pitch (sfx step pitch << 16) (post-fx)
+0x0024: current pitch
+0x0028: current vol
+0x002c: noise seed
+0x0030: noise value
+0x0034: noise value
+0x0038: sfx step vol
+0x003c: sfx step pitch
+0x0040: sfx step waveform
+0x0044: sfx step effect cmd
+0x0048: sfx step filter byte
+0x004c: ?
+0x0050: detune, 1 or 2 if enabled
+0x0054: buzz, 1 if enabled
+0x0058: noiz, 1 if enabled, 2 if (CHANNEL STATE +0x2ee8 < 0xb)
+0x005c: reverb, 1 or 2 if enabled
```

## NOTE LOOKUP TABLE

```
+0x0000: 0000020b 523Hz C 5
+0x0004: 0000022a 554Hz C#5
+0x0008: 0000024b 587Hz D 5
+0x000c: 0000026e 622Hz D#5
+0x0010: 00000293 659Hz E 5
+0x0014: 000002ba 698Hz F 5
+0x0018: 000002e4 740Hz F#5
+0x001c: 00000310 784Hz G 5
+0x0020: 0000033f 831Hz G#5
+0x0024: 00000370 880Hz A 5
+0x0028: 000003a4 932Hz A#5
+0x002c: 000003d8 984Hz B 5
```
