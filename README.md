## CodeRed Generator (UE3 SDK Generator)

CodeRed Generator is a C++20 Unreal Engine 3 SDK generator. It initializes UE3 globals and generates a plug‑and‑play SDK for your target game. This fork adds in‑DLL runtime scanning for GObjects and GNames so you never need to hardcode or recompile offsets when the game updates.

### Highlights

- Runtime offset scanning in DLL (no manual offsets)
- Pattern and legacy fallback search mirrored from `main.py`
- Auto‑initializes `GObjects` and `GNames` on injection
- Clean project layout and filters aligned to folders
- Configurable SDK output and Windows macros support

### Project Layout

- `src/` — core sources (`dllmain.cpp`, `ScanOffsets.cpp`, `Member.cpp`, `Printer.cpp`, `pch.cpp`)
- `include/` — public headers (`dllmain.hpp`, `ScanOffsets.hpp`, `Member.hpp`, `Printer.hpp`, `pch.hpp`, `Changelog.hpp`)
- `Engine/` — game engine wiring and per‑game settings (`Engine.hpp`, `RLSDK/*`)
- `x64/` — build artifacts (Visual Studio)

### Build (Visual Studio)

- Toolset: v143, C++20
- Platform: x64
- Configuration: `Release|x64` (builds a DLL)
- The project already includes `Psapi.lib` and sets `include/` in additional include directories

### Configure

- Edit `Engine/RLSDK/Configuration.cpp`:
  - `UsingOffsets = false` to use runtime scanning (default)
  - Set `Directory`, `GameName`, `GameNameShort`, `GameVersion`
  - Adjust alignment, flags, and ProcessEvent settings as needed

### Use

1. Build the DLL in `Release|x64`.
2. Inject into the running game process.
3. On attach, the DLL scans for GObjects/GNames using signatures from `main.py`:
   - Current and legacy patterns with RIP‑relative extraction
   - Sanity check to normalize the expected 0x48 spacing
4. When found, it initializes globals and generates the SDK under `Configuration::Directory`.

An info dialog shows the resolved addresses and offsets relative to the module base.

### Notes

- Globals are validated before generation; errors display a dialog with corrective hints.
- You can still switch to fixed offsets by setting `UsingOffsets = true` and providing offsets.
- Windows macro renaming and other SDK output settings are available in config.

### Credits

- Original UE3 generator by TheFeckless
- Modernization and maintenance by Zemi
