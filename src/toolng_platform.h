#ifndef TOOLNG_PLATFORM_H
#define TOOLNG_PLATFORM_H

#include <stdint.h>

typedef enum ToolNGArchitecture {
    TOOLNG_ARCH_UNKNOWN = 0,
    TOOLNG_ARCH_X86,
    TOOLNG_ARCH_X86_64,
    TOOLNG_ARCH_ARM32,
    TOOLNG_ARCH_AARCH64,
    TOOLNG_ARCH_RISCV32,
    TOOLNG_ARCH_RISCV64,
    TOOLNG_ARCH_E2K
} ToolNGArchitecture;

typedef struct ToolNGPlatformInfo {
    ToolNGArchitecture architecture;
    uint32_t pointer_bits;
    uint32_t little_endian;
    uint32_t elf_machine;
} ToolNGPlatformInfo;

ToolNGPlatformInfo toolng_platform_info(void);
const char *toolng_architecture_name(ToolNGArchitecture architecture);

#endif
