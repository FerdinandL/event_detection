#include "mmap.hpp"

#include <QDebug>
#include <stdexcept>
#include <iostream>

namespace storage {
    
    namespace util {
        
        MMap::MMap(size_t size, void *tentative_base):
        size(size)
        {
            base = mmap(tentative_base,
                        size,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANON | MAP_NORESERVE,
                        -1,
                        0);
            
            //    std::cout << "mmap base:           " << base << std::endl;
            //    std::cout << "mmap tentative base: " << tentative_base << std::endl;
            
            if (tentative_base && (base != tentative_base)) {
                qDebug() << "mmap on tentative base address failed!";
                exit(1);
//                throw std::runtime_error("mmap on tentative base address failed!");
            }

//            qDebug() << base << (void*)((char*)base+size) << size / (1<<22);
            
            
            if (!base || base == MAP_FAILED) {
                qDebug() << "mmap failed";
                exit(1);
//                throw std::runtime_error("mmap failed");
            }
        }
        
        MMap::~MMap() {
            munmap(base, size);
        }
        
    }
    
}
