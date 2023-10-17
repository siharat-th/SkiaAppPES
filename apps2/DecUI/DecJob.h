#ifndef __DEC_JOB_H__
#define __DEC_JOB_H__

#include <string>
#include <vector>
#include <functional>

class DecJob {
public:
    DecJob( std::function<bool(DecWidget &widget)>jobProc );
    ~DecJob();

    bool isEnded;

    std::function<bool(DecWidget &widget)>_jobProc;
};

#endif