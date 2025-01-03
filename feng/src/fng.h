#pragma once

#define MAX_NO_MODEL_INSTANCES 10
#define MAX_NO_MODEL2D_INSTANCES 10

#define MAX_NO_TEXTURE_UNITS 32
// -1 (slot) -1 (shadowmap)
#define MAX_TEXTURE_SLOT_M MAX_NO_TEXTURE_UNITS - 2
#define NULL_TEXTURE_IDX 255

#define MAX_POINT_LIGHTS 1
#define MAX_SPOT_LIGHTS 1

typedef uint16_t inst_flag_type;

#define SHADOWMAP_SIZE 1 * 1024

#define SHADOWMAP_TEXTURE_SLOT MAX_NO_TEXTURE_UNITS - 1