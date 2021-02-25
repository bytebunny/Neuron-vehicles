#ifndef VEHICLE_TYPES_H
#define VEHICLE_TYPES_H

namespace vehicleTypes
{
    // inline constexpr requires C++17
    inline constexpr int fixed_light { 0 };
    inline constexpr int shy_light_seeker { 1 };
    inline constexpr int aggress_light_seeker { 2 };
    inline constexpr int aggress_light_phobe { 3 };
    inline constexpr int shy_light_phobe { 4 };
    inline constexpr int prey { 5 };
    inline constexpr int bright_prey { 6 };
    inline constexpr int predator { 7 };
    inline constexpr int speculatrix { 8 };
    inline constexpr int test_vehicle { 9 };
}

#endif // VEHICLE_TYPES_H
