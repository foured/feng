#pragma once

#include <cstdint>

namespace feng {
    class renderer{
    public:
        static void get_static_values();
        static int32_t get_max_no_texture_units();
        static int32_t get_shadowmap_texture_slot();
        static int32_t get_max_no_free_texture_units();

    private:
        static int32_t _max_no_texture_units;

    };
}