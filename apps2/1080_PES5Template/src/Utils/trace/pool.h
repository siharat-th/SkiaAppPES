//
//  pool.h
//  all
//
//  Created by SIHARAT THAMMAYA on 27/9/2565 BE.
//

#ifndef pool_h
#define pool_h

// Pool memory allocation

#include <stdlib.h>

template <typename T>
class pool {
    
public:
    
    pool()
    {
        cblock = 0;
        size = sizeof(T) > sizeof(void *) ? sizeof(T) : sizeof(void *);
        next = NULL;
        for (int k = 0; k < 64; k++) {
            block[k] = NULL;
        }
    }
    
    ~pool()
    {
        for (int k = 0; k < cblock; k++) {
            free(block[k]);
        }
    }
    
    T *draw()
    {
        if (!next) addblock();
        void *p = next;
        next = *(void **)p;
        return (T *) p;
    }
    
    void drop(T *p)
    {
        *(void **)p = next;
        next = (void *) p;
    }
    
private:
    
    int size;
    int cblock;
    void *block[64]; //enough to store unlimited number of objects, if 64 is changed: see constructor too
    void *next;
    
    void addblock()
    {
        int i = cblock++;
        int blocksize = 1 << (6 + (i/2));
        //printf("pool allocating block: %d (size:%d)...", i, blocksize);//debug
        block[i] = (void *)malloc(blocksize * size);
        if (!block[i]){
//            throw std::bad_alloc();
            return;
        }
        char *p = (char *)block[i];
        for (int k = 0; k < blocksize - 1; k++)
        {
            *(void**)p = (void *)(p + size);
            p += size;
        }
        *(void **)p = next;
        next = block[i];
        //printf("done\n");//debug
    }
    
};

#endif /* pool_h */
