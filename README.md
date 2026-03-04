# Keypad Firmware

Zephyr-based firmware for a 4x4 matrix keypad on the BL5340 DVK (`bl5340_dvk/nrf5340/cpuapp`).

## Features

- 4x4 keypad matrix scan loop
- Debounce handling
- UART logging of detected row/column and key value

## Pin Mapping (GPIO0)

### Rows (outputs)
- R1: P0.06
- R2: P0.07
- R3: P0.08
- R4: P0.09

### Columns (inputs, pull-down)
- C1: P0.00
- C2: P0.01
- C3: P0.04
- C4: P0.05

## Build

```bash
west build --build-dir build . --pristine --board bl5340_dvk/nrf5340/cpuapp
```

## Flash

```bash
west flash -d build --dev-id 1050051597
```

## Serial Output

Expected logs while pressing keys:

- `R<row> C<col>`
- `Key pressed: <key>`
