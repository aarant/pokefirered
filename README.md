# Pokémon FireRed and LeafGreen

This is a fork of the [matching decompilation](https://github.com/pret/pokefirered) at [PRET](https://github.com/pret).

This branch, `mew-ram-event`, builds a binary that may be called (i.e, by ACE) to write a Mew-under-the-truck event into the savefile's ram script.

The following files are relevant:
- [mew_script.s](data/mew_script.s) - 'Bootstrap' script, contains a function `SetMewEventToRamScript` to write to the RAM script, and the script for the Vermilion City sailor.
- [ram_script.s](data/ram_script.s) - Code + map events + scripting to make the event happen. This is written to `0x0203FBC0` by `mew_script.s`.
- [ld_script.ld](ld_script.ld) - Sets up memory addresses so that `ram_script.s` has a VMA in RAM, but an LMA in ROM (see https://sourceware.org/binutils/docs/ld/Output-Section-LMA.html)

To build:
```bash
make
```

which will produce two files: `pokefirered.gba` and `ram_event.bin`. The ROM will also contain the contents of `ram_event.bin` starting at `0x09000000`.

To verify that the ROM is not modified (past `0x09000000` anyway), run
```bash
make compare
```

which will truncate the ROM to 16 MiB.

To set up the repository, see [INSTALL.md](INSTALL.md).
