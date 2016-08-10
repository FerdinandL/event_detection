#pragma once

#include <unistd.h>
#include <cstddef>
#include <stdint.h>
#include <sys/mman.h>


namespace storage {
    
    namespace util {
        
        //-----------------------------------------------------------------------------
        // MMap
        // Simple wrapper over std mmap
        //-----------------------------------------------------------------------------
        
        struct MMap {
        public:
            MMap(size_t size, void *tentative_base=nullptr);
            ~MMap();
        public:
            void    *base;
            size_t   size;
        };
        
    }
    
}
