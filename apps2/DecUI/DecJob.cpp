#include <string>
#include <vector>
#include <functional>
#include "DecWidget.h"
#include "DecJob.h"

DecJob::DecJob( std::function<bool(DecWidget &widget)>jobProc ) {
    _jobProc = jobProc;
    isEnded = false;
}

DecJob::~DecJob() {
    
}