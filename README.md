## RLSDK Generator V2

RLSDK Generator V2 is a C++20 Unreal Engine 3 SDK generator. It initializes UE3 globals and generates a plug‑and‑play SDK for your target game. This fork adds in‑DLL runtime scanning for GObjects and GNames so you never need to hardcode or recompile offsets when the game updates.

### Highlights

- Runtime offset scanning in DLL (no manual offsets)
- Pattern and legacy fallback search mirrored from `main.py`
- Auto‑initializes `GObjects` and `GNames` on injection
- Clean project layout and filters aligned to folders
- Configurable SDK output and Windows macros support


  - Adjust alignment, flags, and ProcessEvent settings as needed

### Use

1. Build the DLL in `Release|x64`.
2. Inject into the running game process.
3. On attach, the DLL scans for GObjects/GNames
   - Current and legacy patterns with RIP‑relative extraction
   - Sanity check to normalize the expected 0x48 spacing
4. When found, it initializes globals and generates the SDK under `Configuration::Directory`.

An info dialog shows the resolved addresses and offsets relative to the module base.

### Notes

- Globals are validated before generation; errors display a dialog with corrective hints.
- You can still switch to fixed offsets by setting `UsingOffsets = true` and providing offsets.
- Windows macro renaming and other SDK output settings are available in config.
