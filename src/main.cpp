#include <cstdio>
#include <cstdlib>
#include <unordered_set>
#include <span>
#include <filesystem>

#include "rabbitizer.hpp"
#include "elfio/elfio.hpp"
#include "fmt/format.h"
#include "fmt/ostream.h"

#include "recomp_port.h"

std::unordered_set<std::string> ignored_funcs {
    // OS initialize functions
    "__createSpeedParam",
    "__osInitialize_common",
    "__osInitialize_autodetect",
    "osInitialize",
    // Audio interface functions
    "osAiGetLength",
    "osAiGetStatus",
    "osAiSetFrequency",
    "osAiSetNextBuffer",
    "__osAiDeviceBusy",
    // Video interface functions
    "osViBlack",
    "osViFade",
    "osViGetCurrentField",
    "osViGetCurrentFramebuffer",
    "osViGetCurrentLine",
    "osViGetCurrentMode",
    "osViGetNextFramebuffer",
    "osViGetStatus",
    "osViRepeatLine",
    "osViSetEvent",
    "osViSetMode",
    "osViSetSpecialFeatures",
    "osViSetXScale",
    "osViSetYScale",
    "osViSwapBuffer",
    "osCreateViManager",
    "viMgrMain",
    "__osViInit",
    "__osViSwapContext",
    "__osViGetCurrentContext",
    // RDP functions
    "osDpGetCounters",
    "osDpSetStatus",
    "osDpGetStatus",
    "osDpSetNextBuffer",
    "__osDpDeviceBusy",
    // RSP functions
    "osSpTaskLoad",
    "osSpTaskStartGo",
    "osSpTaskYield",
    "osSpTaskYielded",
    "__osSpDeviceBusy",
    "__osSpGetStatus",
    "__osSpRawStartDma",
    "__osSpRawReadIo",
    "__osSpRawWriteIo",
    "__osSpSetPc",
    "__osSpSetStatus",
    // Controller functions
    "osContGetQuery",
    "osContGetReadData",
    "osContInit",
    "osContReset",
    "osContSetCh",
    "osContStartQuery",
    "osContStartReadData",
    "__osContAddressCrc",
    "__osContDataCrc",
    "__osContGetInitData",
    "__osContRamRead",
    "__osContRamWrite",
    // EEPROM functions
    "osEepromLongRead",
    "osEepromLongWrite",
    "osEepromProbe",
    "osEepromRead",
    "osEepromWrite",
    "__osEepStatus",
    // Rumble functions
    "osMotorInit",
    "osMotorStart",
    "osMotorStop",
    // PFS functions
    "osPfsAllocateFile",
    "osPfsChecker",
    "osPfsDeleteFile",
    "osPfsFileState",
    "osPfsFindFile",
    "osPfsFreeBlocks",
    "osPfsGetLabel",
    "osPfsInit",
    "osPfsInitPak",
    "osPfsIsPlug",
    "osPfsNumFiles",
    "osPfsRepairId",
    "osPfsReadWriteFile",
    "__osPackEepReadData",
    "__osPackEepWriteData",
    "__osPackRamReadData",
    "__osPackRamWriteData",
    "__osPackReadData",
    "__osPackRequestData",
    "__osPfsGetInitData",
    "__osPfsGetOneChannelData",
    "__osPfsGetStatus",
    "__osPfsRequestData",
    "__osPfsRequestOneChannel",
    "__osPfsCreateAccessQueue",
    // Low level serial interface functions
    "__osSiDeviceBusy",
    "__osSiGetStatus",
    "__osSiRawStartDma",
    "__osSiRawReadIo",
    "__osSiRawWriteIo",
    "__osSiCreateAccessQueue",
    "__osSiGetAccess",
    "__osSiRelAccess",
    // Parallel interface (cartridge, DMA, etc.) functions
    "osCartRomInit",
    "osLeoDiskInit",
    "osCreatePiManager",
    "__osDevMgrMain",
    "osPiGetCmdQueue",
    "osPiGetStatus",
    "osPiReadIo",
    "osPiStartDma",
    "osPiWriteIo",
    "osEPiGetDeviceType",
    "osEPiStartDma",
    "osEPiWriteIo",
    "osEPiReadIo",
    "osPiRawStartDma",
    "osPiRawReadIo",
    "osPiRawWriteIo",
    "osEPiRawStartDma",
    "osEPiRawReadIo",
    "osEPiRawWriteIo",
    "__osPiRawStartDma",
    "__osPiRawReadIo",
    "__osPiRawWriteIo",
    "__osEPiRawStartDma",
    "__osEPiRawReadIo",
    "__osEPiRawWriteIo",
    "__osPiDeviceBusy",
    "__osPiCreateAccessQueue",
    "__osPiGetAccess",
    "__osPiRelAccess",
    "__osLeoAbnormalResume",
    "__osLeoInterrupt",
    "__osLeoResume",
    // Threading functions
    "osCreateThread",
    "osStartThread",
    "osStopThread",
    "osDestroyThread",
    "osYieldThread",
    "osSetThreadPri",
    "osGetThreadPri",
    "osGetThreadId",
    "__osDequeueThread",
    // Message Queue functions
    "osCreateMesgQueue",
    "osSendMesg",
    "osJamMesg",
    "osRecvMesg",
    "osSetEventMesg",
    // Timer functions
    "osStartTimer",
    "osSetTimer",
    "osStopTimer",
    "osGetTime",
    "__osInsertTimer",
    "__osTimerInterrupt",
    "__osTimerServicesInit",
    "__osSetTimerIntr",
    // exceptasm functions
    "__osExceptionPreamble",
    "__osException",
    "send_mesg",
    "handle_CpU",
    "__osEnqueueAndYield",
    "__osEnqueueThread",
    "__osPopThread",
    "__osNop",
    "__osDispatchThread",
    "__osCleanupThread",
    "osGetCurrFaultedThread",
    "osGetNextFaultedThread",
    // interrupt functions
    "osSetIntMask",
    "osGetIntMask",
    "__osDisableInt",
    "__osRestoreInt",
    "__osSetGlobalIntMask",
    "__osResetGlobalIntMask",
    // TLB functions
    "osMapTLB",
    "osUnmapTLB",
    "osUnmapTLBAll",
    "osSetTLBASID",
    "osMapTLBRdb",
    "osVirtualToPhysical",
    "__osGetTLBHi",
    "__osGetTLBLo0",
    "__osGetTLBLo1",
    "__osGetTLBPageMask",
    "__osGetTLBASID",
    "__osProbeTLB",
    // Coprocessor 0/1 functions
    "__osSetCount",
    "osGetCount",
    "__osSetSR",
    "__osGetSR",
    "__osSetCause",
    "__osGetCause",
    "__osSetCompare",
    "__osGetCompare",
    "__osSetConfig",
    "__osGetConfig",
    "__osSetWatchLo",
    "__osGetWatchLo",
    "__osSetFpcCsr",
    // Cache funcs
    "osInvalDCache",
    "osInvalICache",
    "osWritebackDCache",
    "osWritebackDCacheAll",
    // Microcodes
    "rspbootTextStart",
    "gspF3DEX2_fifoTextStart",
    "gspS2DEX2_fifoTextStart",
    "gspL3DEX2_fifoTextStart",
};

std::unordered_set<std::string> renamed_funcs{
    "sincosf",
    "sqrtf",
    "memcpy",
    "memset",
    "strchr",
    "bzero",
};

// Functions that weren't declared properly and thus have no size in the elf
std::unordered_map<std::string, size_t> unsized_funcs{
    { "guMtxF2L", 0x64 },
    { "guScaleF", 0x48 },
    { "guTranslateF", 0x48 },
    { "guMtxIdentF", 0x48 },
    { "sqrtf", 0x8 },
    { "guMtxIdent", 0x4C },
};

int main(int argc, char** argv) {
    if (argc != 3) {
        fmt::print("Usage: {} [input elf file] [entrypoint RAM address]\n", argv[0]);
        std::exit(EXIT_SUCCESS);
    }

    ELFIO::elfio elf_file;
    RabbitizerConfig_Cfg.pseudos.pseudoMove = false;
    RabbitizerConfig_Cfg.pseudos.pseudoBeqz = false;
    RabbitizerConfig_Cfg.pseudos.pseudoBnez = false;

    auto exit_failure = [] (const std::string& error_str) {
        fmt::print(stderr, error_str);
        std::exit(EXIT_FAILURE);
    };

    std::string elf_name{ argv[1] };

    if (!elf_file.load(elf_name)) {
        exit_failure("Failed to load provided elf file\n");
    }

    char* end;
    const uint32_t entrypoint = (uint32_t)strtoul(argv[2], &end, 0);
    if (argv[2] == end) {
        exit_failure("Invalid entrypoint value: " + std::string(argv[2]) + "\n");
    }

    if (elf_file.get_class() != ELFIO::ELFCLASS32) {
        exit_failure("Incorrect elf class\n");
    }

    if (elf_file.get_encoding() != ELFIO::ELFDATA2MSB) {
        exit_failure("Incorrect endianness\n");
    }

    // Pointer to the symbol table section
    ELFIO::section* symtab_section = nullptr;
    // ROM address of each section
    std::vector<ELFIO::Elf_Xword> section_rom_addrs{};

    RecompPort::Context context{};
    section_rom_addrs.resize(elf_file.sections.size());
    context.functions.reserve(1024);
    context.rom.reserve(8 * 1024 * 1024);

    // Iterate over every section to record rom addresses and find the symbol table
    fmt::print("Sections\n");
    for (const std::unique_ptr<ELFIO::section>& section : elf_file.sections) {
        //fmt::print("  {}: {} @ 0x{:08X}, 0x{:08X}\n", section->get_index(), section->get_name(), section->get_address(), context.rom.size());
        // Set the rom address of this section to the current accumulated ROM size
        section_rom_addrs[section->get_index()] = context.rom.size();
        // If this section isn't bss (SHT_NOBITS) and ends up in the rom (SHF_ALLOC), copy this section into the rom
        if (section->get_type() != ELFIO::SHT_NOBITS && section->get_flags() & ELFIO::SHF_ALLOC) {
            size_t cur_rom_size = context.rom.size();
            context.rom.resize(context.rom.size() + section->get_size());
            std::copy(section->get_data(), section->get_data() + section->get_size(), &context.rom[cur_rom_size]);
        }
        // Check if this section is the symbol table and record it if so
        if (section->get_type() == ELFIO::SHT_SYMTAB) {
            symtab_section = section.get();
        }
    }

    // If no symbol table was found then exit
    if (symtab_section == nullptr) {
        exit_failure("No symbol table section found\n");
    }

    ELFIO::symbol_section_accessor symbols{ elf_file, symtab_section };

    fmt::print("Num symbols: {}\n", symbols.get_symbols_num());

    bool found_entrypoint_func = false;

    for (int sym_index = 0; sym_index < symbols.get_symbols_num(); sym_index++) {
        std::string   name;
        ELFIO::Elf64_Addr    value;
        ELFIO::Elf_Xword     size;
        unsigned char bind;
        unsigned char type;
        ELFIO::Elf_Half      section_index;
        unsigned char other;
        bool ignored = false;

        // Read symbol properties
        symbols.get_symbol(sym_index, name, value, size, bind, type,
            section_index, other);

        // Check if this symbol is unsized and if so populate its size from the unsized_funcs map
        if (size == 0) {
            if (value == entrypoint && type == ELFIO::STT_FUNC) {
                found_entrypoint_func = true;
                size = 0x50; // dummy size for entrypoints, should cover them all
                name = "recomp_entrypoint";
            } else {
                auto size_find = unsized_funcs.find(name);
                if (size_find != unsized_funcs.end()) {
                    size = size_find->second;
                    type = ELFIO::STT_FUNC;
                }
            }
        }

        if (ignored_funcs.contains(name)) {
            name = name + "_recomp";
            ignored = true;
        }

        // Check if this symbol is a function or has no type (like a regular glabel would)
        // Symbols with no type have a dummy entry created so that their symbol can be looked up for function calls
        if (ignored || type == ELFIO::STT_FUNC || type == ELFIO::STT_NOTYPE || type == ELFIO::STT_OBJECT) {
            if (renamed_funcs.contains(name)) {
                name = "_" + name;
                ignored = false;
            }
            if (section_index < section_rom_addrs.size()) {
                auto section_rom_addr = section_rom_addrs[section_index];
                auto section_offset = value - elf_file.sections[section_index]->get_address();
                const uint32_t* words = reinterpret_cast<const uint32_t*>(elf_file.sections[section_index]->get_data() + section_offset);
                uint32_t vram = static_cast<uint32_t>(value);
                uint32_t num_instructions = type == ELFIO::STT_FUNC ? size / 4 : 0;
                context.functions_by_vram[vram].push_back(context.functions.size());
                context.functions.emplace_back(
                    vram,
                    static_cast<uint32_t>(section_offset + section_rom_addr),
                    std::span{ words, num_instructions },
                    std::move(name),
                    ignored
                );
            } else {
                uint32_t vram = static_cast<uint32_t>(value);
                context.functions_by_vram[vram].push_back(context.functions.size());
                context.functions.emplace_back(
                    vram,
                    0,
                    std::span<const uint32_t>{},
                    std::move(name),
                    ignored
                );
            }
        }
    }

    if (!found_entrypoint_func) {
        exit_failure("Could not find entrypoint function\n");
    }

    fmt::print("Function count: {}\n", context.functions.size());

    std::ofstream func_lookup_file{ "test/funcs/lookup.cpp" };
    std::ofstream func_header_file{ "test/funcs/funcs.h" };

    fmt::print(func_lookup_file,
        "#include <utility>\n"
        "#include \"recomp.h\"\n"
        "#include \"funcs.h\"\n"
        "\n"
        "std::pair<uint32_t, recomp_func_t*> funcs[] {{\n"
    );

    fmt::print(func_header_file,
        "#include \"recomp.h\"\n"
        "\n"
        "#ifdef __cplusplus\n"
        "extern \"C\" {{\n"
        "#endif\n"
        "\n"
    );

    //#pragma omp parallel for
    for (size_t i = 0; i < context.functions.size(); i++) {
        const auto& func = context.functions[i];

        if (!func.ignored && func.words.size() != 0) {
            fmt::print(func_header_file,
                "void {}(uint8_t* restrict rdram, recomp_context* restrict ctx);\n", func.name);
            fmt::print(func_lookup_file,
                "    {{ 0x{:08X}u, {} }},\n", func.vram, func.name);
            if (RecompPort::recompile_function(context, func, "test/funcs/" + func.name + ".c") == false) {
                func_lookup_file.clear();
                fmt::print(stderr, "Error recompiling {}\n", func.name);
                std::exit(EXIT_FAILURE);
            }
        }
    }

    fmt::print(func_lookup_file,
        "}};\n"
        "extern const size_t num_funcs = sizeof(funcs) / sizeof(funcs[0]);\n"
        "\n"
        "gpr get_entrypoint_address() {{ return (gpr)(int32_t)0x{:08X}u; }}\n"
        "\n"
        "const char* get_rom_name() {{ return \"{}\"; }}\n"
        "\n",
        entrypoint,
        std::filesystem::path{ elf_name }.replace_extension(".z64").string()
    );

    fmt::print(func_header_file,
        "\n"
        "#ifdef __cplusplus\n"
        "}}\n"
        "#endif\n"
    );

    return 0;
}
