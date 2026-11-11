#include "toolng_platform.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    ToolNGPlatformInfo info = toolng_platform_info();
    assert(info.pointer_bits == 32u || info.pointer_bits == 64u);
    assert(info.little_endian <= 1u);
    assert(strcmp(toolng_architecture_name(info.architecture), "unknown") != 0 ||
           info.architecture == TOOLNG_ARCH_UNKNOWN);
    if (info.architecture == TOOLNG_ARCH_E2K) {
        assert(info.pointer_bits == 64u);
        assert(info.little_endian == 1u);
        assert(info.elf_machine == 175u);
    } else {
        assert(info.elf_machine == 0u);
    }
    return 0;
}
