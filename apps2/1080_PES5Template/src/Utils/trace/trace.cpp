//
//  trace.cpp
//  all
//
//  Created by SIHARAT THAMMAYA on 29/9/2565 BE.
//

#include "trace.hpp"
#include "siox.hpp"
#include "imagemap.hpp"

class TraceSioxObserver : public SioxObserver
{
public:
    
    TraceSioxObserver (void *contextArg) :
    SioxObserver(contextArg)
    {}
    
    virtual ~TraceSioxObserver ()
    { }
    
    virtual bool progress(float /*percentCompleted*/)
    {
        //Tracer *tracer = (Tracer *)context;
        //## Allow the GUI to update
//        Gtk::Main::iteration(false); //at least once, non-blocking
//        while( Gtk::Main::events_pending() )
//            Gtk::Main::iteration();
        return true;
    }
    
    virtual void error(const std::string &/*msg*/)
    {
        //Tracer *tracer = (Tracer *)context;
    }
    
};
