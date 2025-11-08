#include "ScanOffsets.hpp"

namespace OffsetScanner
{
    static const uint8_t GObjects_Current[]   = { 0x48, 0x8B, 0xC8, 0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x0C, 0xC8 };
    static const uint8_t GNames_Current1[]    = { 0x48, 0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x0C, 0xC1 };
    static const uint8_t GNames_Current2[]    = { 0x49, 0x63, 0x06, 0x48, 0x8D, 0x55, 0xE8, 0x48, 0x8B, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x0C, 0xC1 };
    static const uint8_t GNames_Legacy1[]     = { 0x75, 0x05, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x85, 0xDB, 0x75, 0x31 };
    static const uint8_t GObjects_Legacy1[]   = { 0xE8, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x5D, 0xBF, 0x48 };

    static constexpr uintptr_t EXPECTED_DIFF = 0x48;

    static uintptr_t ScanMemory(HMODULE module, const uint8_t* pattern, size_t length)
    {
        MODULEINFO mi{};
        if (!K32GetModuleInformation(GetCurrentProcess(), module, &mi, sizeof(mi)))
        {
            return 0;
        }

        const auto* base = reinterpret_cast<const uint8_t*>(module);
        const size_t size = mi.SizeOfImage;

        for (size_t i = 0; i + length <= size; ++i)
        {
            bool match = true;
            for (size_t j = 0; j < length; ++j)
            {
                const uint8_t want = pattern[j];
                if (want != 0x00 && base[i + j] != want)
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                return reinterpret_cast<uintptr_t>(base + i);
            }
        }

        return 0;
    }

    static uintptr_t ExtractRelTarget(uintptr_t instr, int relIndex)
    {
        const int32_t disp = *reinterpret_cast<const int32_t*>(instr + relIndex);
        return static_cast<uintptr_t>((instr + relIndex + sizeof(int32_t)) + disp);
    }

    bool Resolve(uintptr_t& outGNames, uintptr_t& outGObjects)
    {
        outGNames = 0;
        outGObjects = 0;

        HMODULE exe = GetModuleHandle(NULL);
        if (!exe) return false;

        // Try current patterns first
        if (!outGObjects)
        {
            const uintptr_t addr = ScanMemory(exe, GObjects_Current, sizeof(GObjects_Current));
            if (addr)
            {
                outGObjects = ExtractRelTarget(addr, 6);
            }
        }

        if (!outGNames)
        {
            const uintptr_t addr = ScanMemory(exe, GNames_Current1, sizeof(GNames_Current1));
            if (addr)
            {
                outGNames = ExtractRelTarget(addr, 3);
            }
        }

        if (!outGNames)
        {
            const uintptr_t addr = ScanMemory(exe, GNames_Current2, sizeof(GNames_Current2));
            if (addr)
            {
                outGNames = ExtractRelTarget(addr, 10);
            }
        }

        // Legacy fallbacks
        if (!outGNames)
        {
            const uintptr_t addr = ScanMemory(exe, GNames_Legacy1, sizeof(GNames_Legacy1));
            if (addr)
            {
                outGNames = ExtractRelTarget(addr, 3);
            }
        }

        if (!outGObjects)
        {
            const uintptr_t addr = ScanMemory(exe, GObjects_Legacy1, sizeof(GObjects_Legacy1));
            if (addr)
            {
                outGObjects = ExtractRelTarget(addr, 1);
            }
        }

        if (outGNames && outGObjects)
        {
            const uintptr_t diff = (outGObjects > outGNames) ? (outGObjects - outGNames) : (outGNames - outGObjects);
            if (diff != EXPECTED_DIFF)
            {
                outGNames = outGObjects - EXPECTED_DIFF;
            }
            return true;
        }

        return false;
    }
}
