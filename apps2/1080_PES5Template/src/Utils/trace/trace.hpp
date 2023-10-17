//
//  trace.hpp
//  all
//
//  Created by SIHARAT THAMMAYA on 29/9/2565 BE.
//

#ifndef trace_hpp
#define trace_hpp

#include <stdio.h>
#include "include/core/SkBitmap.h"
#include "pesEngine.h"

class TracingEngineResult
{
    
public:
    
    TracingEngineResult(const std::string &theStyle,
                        const pesPath &thePathData,
                        long theNodeCount) :
    style(theStyle),
    pathData(thePathData),
    nodeCount(theNodeCount)
    {}
    
    TracingEngineResult(const TracingEngineResult &other)
    { assign(other); }
    
    virtual TracingEngineResult &operator=(const TracingEngineResult &other)
    { assign(other); return *this; }
    
    
    virtual ~TracingEngineResult()
    { }
    
    std::string getStyle()
    { return style; }
    
    pesPath getPathData()
    { return pathData; }
    
    long getNodeCount()
    { return nodeCount; }
    
private:
    
    void assign(const TracingEngineResult &other)
    {
        style = other.style;
        pathData = other.pathData;
        nodeCount = other.nodeCount;
    }
    
    std::string style;
    
    pesPath pathData;
    
    long nodeCount;
    
};



class TracingEngine
{
    
public:
    
    TracingEngine()
    {}
    
    virtual ~TracingEngine()
    {}
    
    virtual  std::vector<TracingEngineResult> trace( const SkBitmap & bitmap )
    {
        std::vector<TracingEngineResult> dummy;
        return dummy;
    }
    
    virtual void abort()
    {}
    
    
    
};

#endif /* trace_hpp */
