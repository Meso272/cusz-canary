/**
 * @file typing.inl
 * @author Jiannan Tian
 * @brief
 * @version 0.4
 * @date 2022-12-22
 *
 * (C) 2022 by Indiana University, Argonne National Laboratory
 *
 */

#include <stdint.h>
#include <stdlib.h>

namespace parsz {
namespace typing {

// clang-format off
template <int BYTEWIDTH> struct Int;
template <> struct Int<1> { typedef int8_t  T; }; 
template <> struct Int<2> { typedef int16_t T; }; 
template <> struct Int<4> { typedef int32_t T; }; 
template <> struct Int<8> { typedef int64_t T; };

template <int BYTEWIDTH> struct UInt;
template <> struct UInt<1> { typedef uint8_t  T; }; 
template <> struct UInt<2> { typedef uint16_t T; }; 
template <> struct UInt<4> { typedef uint32_t T; }; 
template <> struct UInt<8> { typedef uint64_t T; };
// clang-format on

}  // namespace typing
}  // namespace parsz

template <int BYTEWIDTH>
struct PN {
    using UI = typename parsz::typing::UInt<BYTEWIDTH>::T;
    using I  = typename parsz::typing::Int<BYTEWIDTH>::T;

    // reference: https://lemire.me/blog/2022/11/25/making-all-your-integers-positive-with-zigzag-encoding/

    UI encode(I& x) { return (2 * x) ^ (x >> (BYTEWIDTH * 8 - 1)); }
    I  decode(UI& x) { return (x >> 1) ^ (-(x & 1)); }
};

template <typename T>
struct OutlierDescriptionGlobalMemory {
    using type = T;
    T*        val;
    uint32_t* idx;
    uint32_t* count;

    void allocate(size_t len, bool device = true)
    {
        if (device) {
            cudaMalloc(&idx, sizeof(uint32_t) * len);
            cudaMalloc(&val, sizeof(T) * len);
            cudaMalloc(&count, sizeof(T) * 1);

            cudaMemset(count, 0x0, sizeof(T) * 1);
        }
        else {
            cudaMallocHost(&idx, sizeof(uint32_t) * len);
            cudaMallocHost(&val, sizeof(T) * len);
            cudaMallocHost(&count, sizeof(T) * 1);

            memset(count, 0x0, sizeof(T) * 1);
        }
    }

    void allocate_managed(size_t len)
    {
        cudaMallocManaged(&idx, sizeof(uint32_t) * len);
        cudaMallocManaged(&val, sizeof(T) * len);
        cudaMallocManaged(&count, sizeof(T) * 1);

        cudaMemset(count, 0x0, sizeof(T) * 1);
    }

    void destroy()
    {
        cudaFree(idx);
        cudaFree(val);
        cudaFree(count);
    }
};
