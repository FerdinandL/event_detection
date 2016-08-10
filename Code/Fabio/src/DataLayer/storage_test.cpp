#include "arena.hpp"

#include <unistd.h>
#include <sys/mman.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <QSettings>

#include "mmap.hpp"
#include "arena.hpp"
#include "DataStore.hpp"


int main() {
    
    DataStoreArena *dsa1 = new DataStoreArena();
    
    dsa1->loadCSVData("taxi", "/tmp/taxi.csv");
    
    // settings
    QSettings settings("/tmp/urbane/meta.ini",QSettings::Format::IniFormat);
    settings.setValue("storage/path", "/tmp/urbane/");
    settings.sync();
    
    // persist everything
    ::storage::io::persist(settings, dsa1->arena);
    
    // create new dsa to bring blocks back
    DataStoreArena *dsa2 = new DataStoreArena();
    
    // bring it back
    ::storage::io::restore(settings, dsa2->arena);
    
    // write spacetime,distance
    void *spacetime = nullptr;
    int spacetime_start;
    int spacetime_end;
    void *distance = nullptr;
    int distance_start;
    int distance_end;
    
    std::cout << "Num blocks spacetime: " << dsa2->getNumBlocks("taxi", "spacetime") << std::endl;
    std::cout << "Num blocks Trip_distance: " << dsa2->getNumBlocks("taxi", "Trip_distance") << std::endl;
    
    
    dsa2->getData("taxi", "spacetime", 0, &spacetime, &spacetime_start, &spacetime_end);
    dsa2->getData("taxi", "Trip_distance", 0, &distance, &distance_start, &distance_end);
    
    std::cout << "spacetime start/end ids: " << spacetime_start << "," << spacetime_end << std::endl;
    std::cout << "Trip_distance start/end ids: " << distance_start << "," << distance_end << std::endl;
    
    
    DataType spacetime_type;
    int spacetime_size;
    int spacetime_numspatial;
    
    DataType distance_type;
    int distance_size;
    int distance_numspatial;
    
    dsa2->getTupleSize("taxi", "spacetime", &spacetime_type, &spacetime_size, &spacetime_numspatial);
    dsa2->getTupleSize("taxi", "Trip_distance", &distance_type, &distance_size, &distance_numspatial);
    
    std::cout << "spacetime info: " << spacetime_type << "," << spacetime_size << "," << spacetime_numspatial << std::endl;
    std::cout << "Trip_distance info: " << distance_type << "," << distance_size << "," << distance_numspatial << std::endl;
    
    // quick and dirty print
    for(int i=0; i<10; ++i) {
        std::cout << uint2double(*((uint64_t*)spacetime+7*i)) << ",";   // space1
        std::cout << uint2double(*((uint64_t*)spacetime+7*i+1)) << ","; // space1
        std::cout << uint2double(*((uint64_t*)spacetime+7*i+2)) << ","; // space2
        std::cout << uint2double(*((uint64_t*)spacetime+7*i+3)) << ","; // space2
        std::cout << (*((uint64_t*)spacetime+7*i+4)) << ",";            // time1
        std::cout << (*((uint64_t*)spacetime+7*i+5)) << ",";            // time2
        std::cout << (*((uint64_t*)spacetime+7*i+6)) << ",";            // index
        std::cout << (*((float*)distance+i)) << std::endl;
    }

    
}

//
//int main() {
//    
//    // arena creation test
//    
//    // one terabyte of virtual memory
//    const size_t arena_size = (1 << 30);
//    
//    // block size
//    // around 256mb
//    const size_t block_size = (1 << 28);
//    
//    // settings
//    QSettings settings("/tmp/urbane/meta.ini",QSettings::Format::IniFormat);
//    //settings.setPath(QSettings::Format::IniFormat, QSettings::Scope::SystemScope, "/tmp/urbane/");
//    settings.setValue("storage/path", "/tmp/urbane/");
//    settings.sync();
//    
//    std::cout << settings.fileName().toStdString() << std::endl;
//    
//    ::storage::util::MMap mmap(arena_size);
//    ::storage::arena::Arena *arena = new ::storage::arena::Arena::Arena(mmap.base, mmap.size);
//    
//    // create one block for dataset 'taxi', column 'time', 4 bytes for each value
//    ::storage::arena::Block *block = arena->create_block(block_size, "taxi", "time", 4);
//    
//    // write random stuff
//    for(int i=0; i<100; i++) {
//        int value = 100+i;
//        //std::cout << value << std::endl;
//        block->write(i, i, &value);
//    }
//    
//    // persist everything
//    ::storage::io::persist(settings, arena);
//    
//    delete arena;
//    
//    
//    // create another arena and bring everything back
//    ::storage::arena::Arena *arena2 = new ::storage::arena::Arena::Arena(mmap.base, mmap.size);
//    ::storage::io::restore(settings, arena2);
//    
//    ::storage::arena::Block *block2 = arena2->get_block("taxi", "time");
//    
//    for(int i=0; i<100; i++) {
//        int value;
//        block2->read(i, &value);
//        std::cout << value << std::endl;
//    }
//    
//}

