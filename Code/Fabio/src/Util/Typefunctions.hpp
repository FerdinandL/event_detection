#ifndef TYPEFUNCTIONS_HPP
#define TYPEFUNCTIONS_HPP

#include <stdint.h>

#ifdef __CUDACC__
    __host__ __device__
#endif
    inline uint64_t double2uint(double f) {
        register uint64_t t(*((uint64_t*) &f));
        return t ^ ((-(t >> 63UL)) | 0x8000000000000000UL);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    inline double uint2double(uint64_t f) {
        register uint64_t u = f ^ (((f >> 63UL) - 1UL) | 0x8000000000000000UL);
        return *((double*) &u);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    inline int64_t uint2long(uint64_t f) {
        register uint64_t u = (f ^ 0x8000000000000000UL);
        return *((int64_t*) &u);
    }

#ifdef __CUDACC__
    __host__ __device__
#endif
    inline uint64_t long2uint(int64_t f) {
        register uint64_t t(*((uint64_t*) &f));
        return (t ^ 0x8000000000000000UL);
    }


#endif // TYPEFUNCTIONS_HPP
