

#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
} PluginVersion;

typedef struct {
    char* author;
    char* name;
    PluginVersion version;
} PluginInfo;


#ifdef __cplusplus
}
#endif
