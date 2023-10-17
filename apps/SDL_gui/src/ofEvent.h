//
//  ofEvent.h
//  EMBrilliance
//
//  Created by Siharat on 7/27/2560 BE.
//  Copyright © 2560 Jimmy Software Co., Ltd. All rights reserved.
//

#ifndef ofEvent_h
#define ofEvent_h

#ifdef _OF_EVENTS_

#include <vector>
#include <functional>
#include <mutex>
#include <thread>
#include <memory>
#include <iterator>

//-------------------------------
//  find the system type --------
//-------------------------------

#if defined( __WIN32__ ) || defined( _WIN32 )
#define TARGET_WIN32
#elif defined( __APPLE_CC__)
#define __ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORES 0
#include <TargetConditionals.h>

#if TARGET_OS_IPHONE || TARGET_IPHONE
#define TARGET_OF_IPHONE
#define TARGET_OF_IOS
#define TARGET_OPENGLES
#include <unistd.h>
#else
#define TARGET_OSX
#endif
#elif defined (__ANDROID__)
#define TARGET_ANDROID
#define TARGET_OPENGLES
#elif defined(__ARMEL__)
#define TARGET_LINUX
#define TARGET_OPENGLES
#define TARGET_LINUX_ARM
#elif defined(__EMSCRIPTEN__)
#define TARGET_EMSCRIPTEN
#define TARGET_OPENGLES
#define TARGET_NO_THREADS
#define TARGET_PROGRAMMABLE_GL
#define TARGET_IMPLEMENTS_URL_LOADER
#else
#define TARGET_LINUX
#endif



#ifdef TARGET_OSX
#ifndef __MACOSX_CORE__
#define __MACOSX_CORE__
#endif
#include <unistd.h>
//#include "GL/glew.h"
#include <OpenGL/gl.h>
#include <ApplicationServices/ApplicationServices.h>

#if defined(__LITTLE_ENDIAN__)
#define TARGET_LITTLE_ENDIAN		// intel cpu
#endif
#endif





#define OF_KEY_MODIFIER 	0x0100
#define OF_KEY_RETURN		13
#define OF_KEY_ESC			27
#define OF_KEY_TAB          9
#define OF_KEY_COMMAND      OF_KEY_SUPER

// http://www.openframeworks.cc/forum/viewtopic.php?t=494
// some issues with keys across platforms:

#ifdef TARGET_OSX
#define OF_KEY_BACKSPACE	127
#define OF_KEY_DEL			8
#else
#define OF_KEY_BACKSPACE	8
#define OF_KEY_DEL			127
#endif

// zach - there are more of these keys, we can add them here...
// these are keys that are not coming through "special keys"
// via glut, but just other keys on your keyboard like

#define OF_KEY_F1			(1 | OF_KEY_MODIFIER)
#define OF_KEY_F2			(2 | OF_KEY_MODIFIER)
#define OF_KEY_F3			(3 | OF_KEY_MODIFIER)
#define OF_KEY_F4			(4 | OF_KEY_MODIFIER)
#define OF_KEY_F5			(5 | OF_KEY_MODIFIER)
#define OF_KEY_F6			(6 | OF_KEY_MODIFIER)
#define OF_KEY_F7			(7 | OF_KEY_MODIFIER)
#define OF_KEY_F8			(8 | OF_KEY_MODIFIER)
#define OF_KEY_F9			(9 | OF_KEY_MODIFIER)
#define OF_KEY_F10			(10 | OF_KEY_MODIFIER)
#define OF_KEY_F11			(11 | OF_KEY_MODIFIER)
#define OF_KEY_F12			(12 | OF_KEY_MODIFIER)
#define OF_KEY_LEFT			(100 | OF_KEY_MODIFIER)
#define OF_KEY_UP			(101 | OF_KEY_MODIFIER)
#define OF_KEY_RIGHT		(102 | OF_KEY_MODIFIER)
#define OF_KEY_DOWN			(103 | OF_KEY_MODIFIER)
#define OF_KEY_PAGE_UP		(104 | OF_KEY_MODIFIER)
#define OF_KEY_PAGE_DOWN	(105 | OF_KEY_MODIFIER)
#define OF_KEY_HOME			(106 | OF_KEY_MODIFIER)
#define OF_KEY_END			(107 | OF_KEY_MODIFIER)
#define OF_KEY_INSERT		(108 | OF_KEY_MODIFIER)
#define OF_KEY_CONTROL		(0x200 | OF_KEY_MODIFIER)
#define OF_KEY_ALT			(0x400 | OF_KEY_MODIFIER)
#define OF_KEY_SHIFT		(0x800 | OF_KEY_MODIFIER)
#define OF_KEY_SUPER		(0x1000 | OF_KEY_MODIFIER)
#define OF_KEY_LEFT_SHIFT	(0x1 | OF_KEY_SHIFT)
#define OF_KEY_RIGHT_SHIFT	(0x2 | OF_KEY_SHIFT)
#define OF_KEY_LEFT_CONTROL	(0x1 | OF_KEY_CONTROL)
#define OF_KEY_RIGHT_CONTROL (0x2 | OF_KEY_CONTROL)
#define OF_KEY_LEFT_ALT		(0x1 | OF_KEY_ALT)
#define OF_KEY_RIGHT_ALT	(0x2 | OF_KEY_ALT)
#define OF_KEY_LEFT_SUPER	(0x1 | OF_KEY_SUPER)
#define OF_KEY_RIGHT_SUPER	(0x2 | OF_KEY_SUPER)
#define OF_KEY_LEFT_COMMAND OF_KEY_LEFT_SUPER
#define OF_KEY_RIGHT_COMMAND OF_KEY_RIGHT_SUPER
// not sure what to do in the case of non-glut apps....

#define OF_MOUSE_BUTTON_1      0
#define OF_MOUSE_BUTTON_2      1
#define OF_MOUSE_BUTTON_3      2
#define OF_MOUSE_BUTTON_4      3
#define OF_MOUSE_BUTTON_5      4
#define OF_MOUSE_BUTTON_6      5
#define OF_MOUSE_BUTTON_7      6
#define OF_MOUSE_BUTTON_8      7
#define OF_MOUSE_BUTTON_LAST   OF_MOUSE_BUTTON_8
#define OF_MOUSE_BUTTON_LEFT   OF_MOUSE_BUTTON_1
#define OF_MOUSE_BUTTON_MIDDLE OF_MOUSE_BUTTON_2
#define OF_MOUSE_BUTTON_RIGHT  OF_MOUSE_BUTTON_3

//--------------------------------------------
//console colors for our logger - shame this doesn't work with the xcode console

#ifdef TARGET_WIN32

#define OF_CONSOLE_COLOR_RESTORE (0 | (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE) )
#define OF_CONSOLE_COLOR_BLACK (0)
#define OF_CONSOLE_COLOR_RED (FOREGROUND_RED)
#define OF_CONSOLE_COLOR_GREEN (FOREGROUND_GREEN)
#define OF_CONSOLE_COLOR_YELLOW (FOREGROUND_RED|FOREGROUND_GREEN)
#define OF_CONSOLE_COLOR_BLUE (FOREGROUND_BLUE)
#define OF_CONSOLE_COLOR_PURPLE (FOREGROUND_RED | FOREGROUND_BLUE )
#define OF_CONSOLE_COLOR_CYAN (FOREGROUND_GREEN | FOREGROUND_BLUE)
#define OF_CONSOLE_COLOR_WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

#else

#define OF_CONSOLE_COLOR_RESTORE (0)
#define OF_CONSOLE_COLOR_BLACK (30)
#define OF_CONSOLE_COLOR_RED (31)
#define OF_CONSOLE_COLOR_GREEN (32)
#define OF_CONSOLE_COLOR_YELLOW (33)
#define OF_CONSOLE_COLOR_BLUE (34)
#define OF_CONSOLE_COLOR_PURPLE (35)
#define OF_CONSOLE_COLOR_CYAN (36)
#define OF_CONSOLE_COLOR_WHITE (37)

#endif

enum ofPixelFormat{
    // grayscale
    OF_PIXELS_GRAY = 0,
    OF_PIXELS_GRAY_ALPHA = 1,
    
    // rgb (can be 8,16 or 32 bpp depending on pixeltype)
    OF_PIXELS_RGB=2,
    OF_PIXELS_BGR=3,
    OF_PIXELS_RGBA=4,
    OF_PIXELS_BGRA=5,
    
    // rgb 16bit
    OF_PIXELS_RGB565=6,
    
    // yuv
    OF_PIXELS_NV12=7,
    OF_PIXELS_NV21=8,
    OF_PIXELS_YV12=9,
    OF_PIXELS_I420=10,
    OF_PIXELS_YUY2=11,
    OF_PIXELS_UYVY=12,
    
    // yuv planes
    OF_PIXELS_Y,
    OF_PIXELS_U,
    OF_PIXELS_V,
    OF_PIXELS_UV,
    OF_PIXELS_VU,
    
    OF_PIXELS_NUM_FORMATS,
    
    OF_PIXELS_UNKNOWN=-1,
    OF_PIXELS_NATIVE=-2
};

#define OF_PIXELS_MONO OF_PIXELS_GRAY
#define OF_PIXELS_R OF_PIXELS_GRAY
#define OF_PIXELS_RG OF_PIXELS_GRAY_ALPHA

enum ofEventOrder{
    OF_EVENT_ORDER_BEFORE_APP=0,
    OF_EVENT_ORDER_APP=100,
    OF_EVENT_ORDER_AFTER_APP=200
};


class ofEventAttendedException: public std::exception{};

template<typename Function, typename Mutex=std::recursive_mutex>
class ofBaseEvent{
public:
    /// \brief Basic constructor enabling an ofBaseEvent.
    ///
    /// \see ofBaseEvent::ofBaseEvent(const ofBaseEvent & mom)
    /// \see ofBaseEvent::enable()
    /// \see ofBaseEvent::disable()
    /// \see ofBaseEvent::isEnabled()
    ofBaseEvent()
    :enabled(true){
    }
    
    /// \brief Copy-constructor for ofBaseEvent.
    ///
    /// \see ofBaseEvent::ofBaseEvent()
    ofBaseEvent(const ofBaseEvent & mom)
    :enabled(mom.enabled){
        std::unique_lock<Mutex> lck(const_cast<ofBaseEvent&>(mom).mtx);
        functions = mom.functions;
    }
    
    /// \brief Overloading the assignment operator.
    ofBaseEvent & operator=(const ofBaseEvent & mom){
        if(&mom==this){
            return *this;
        }
        std::unique_lock<Mutex> lck(const_cast<ofBaseEvent&>(mom).mtx);
        std::unique_lock<Mutex> lck2(mtx);
        functions = mom.functions;
        enabled = mom.enabled;
        return *this;
    }
    
    /// \brief Enable an event.
    ///
    /// \see ofBaseEvent::disable()
    /// \see ofBaseEvent::isEnabled()
    void enable() {
        enabled = true;
    }
    
    /// \brief Disable an event.
    ///
    /// \see ofBaseEvent::enable()
    /// \see ofBaseEvent::isEnabled()
    void disable() {
        enabled = false;
    }
    
    /// \brief Check whether an event is enabled or not.
    ///
    /// \returns true if enables
    /// \see ofBaseEvent::enable()
    /// \see ofBaseEvent::disable()
    bool isEnabled() const {
        return enabled;
    }
    
    std::size_t size() const {
        return functions.size();
    }
    
protected:
    template<typename TFunction>
    void add(TFunction && f){
        std::unique_lock<Mutex> lck(mtx);
        auto it = functions.begin();
        for(; it!=functions.end(); ++it){
            if((*it)->priority>f->priority) break;
        }
        functions.emplace(it, f);
    }
    
    template<typename TFunction>
    void remove(const TFunction & function){
        std::unique_lock<Mutex> lck(mtx);
        auto it = functions.begin();
        for(; it!=functions.end(); ++it){
            auto f = *it;
            if(*f == *function){
                f->disable();
                functions.erase(it);
                break;
            }
        }
    }
    
    Mutex mtx;
    std::vector<std::shared_ptr<Function>> functions;
    bool enabled;
};

/*! \cond PRIVATE */
namespace of{
    namespace priv{
        class NoopMutex{
        public:
            void lock(){}
            void unlock(){}
        };
        
        class BaseFunctionId{
        public:
            virtual ~BaseFunctionId(){};
            virtual bool operator==(const BaseFunctionId &) const = 0;
        };
        
        template<typename T, class Mutex>
        class Function{
        public:
            Function(int priority, std::function<bool(const void*,T&)> function,  std::unique_ptr<BaseFunctionId>&& id )
            :priority(priority)
            ,id(std::move(id))
            ,function(function){}
            
            bool operator==(const Function<T,Mutex> & f) const{
                return f.priority == priority && *id == *f.id;
            }
            
            inline bool notify(const void*s,T&t){
                std::unique_lock<Mutex> lck(mtx);
                try{
                    return function(s,t);
                }catch(std::bad_function_call &){
                    return false;
                }
            }
            
            inline void disable(){
                std::unique_lock<Mutex> lck(mtx);
                function = nullptr;
            }
            
            int priority;
            std::unique_ptr<BaseFunctionId> id;
            
        private:
            std::function<bool(const void*,T&)> function;
            Mutex mtx;
        };
        
        template<class Mutex>
        class Function<void,Mutex>{
        public:
            Function(int priority, std::function<bool(const void*)> function,  std::unique_ptr<BaseFunctionId> && id )
            :priority(priority)
            ,id(std::move(id))
            ,function(function){}
            
            bool operator==(const Function<void,Mutex> & f) const{
                return f.priority == priority && *id == *f.id;
            }
            
            inline bool notify(const void*s){
                std::unique_lock<Mutex> lck(mtx);
                try{
                    return function(s);
                }catch(std::bad_function_call &){
                    return false;
                }
            }
            
            inline void disable(){
                std::unique_lock<Mutex> lck(mtx);
                function = nullptr;
            }
            
            int priority;
            std::unique_ptr<BaseFunctionId> id;
        private:
            std::function<bool(const void*)> function;
            Mutex mtx;
        };
    }
}
/*! \endcond */

template<typename T, typename Mutex=std::recursive_mutex>
class ofEvent: public ofBaseEvent<of::priv::Function<T,Mutex>,Mutex>{
protected:
    typedef of::priv::Function<T,Mutex> Function;
    typedef std::shared_ptr<Function> FunctionPtr;
    
    template<class TObj, typename TMethod>
    class FunctionId: public of::priv::BaseFunctionId{
    public:
        TObj * listener;
        TMethod method;
        
        FunctionId(TObj * listener, TMethod method)
        :listener(listener)
        ,method(method){
            
        }
        
        BaseFunctionId * clone() const{
            return new FunctionId<TObj,TMethod>(listener, method);
        }
        
        template<typename F>
        bool operator==(const F & f1) const{
            return f1.listener == this->listener && f1.method == this->method;
        }
        
        bool operator==(const BaseFunctionId & f) const{
            const auto * other = dynamic_cast<const FunctionId<TObj,TMethod>*>(&f);
            return other && other->listener == this->listener && other->method == this->method;
        }
    };
    
    template<class TObj, typename TMethod>
    std::unique_ptr<FunctionId<TObj,TMethod>> make_function_id(TObj * listener, TMethod method){
        return std::unique_ptr<FunctionId<TObj,TMethod>>(new FunctionId<TObj,TMethod>(listener,method));
    }
    
    template<class TObj>
    FunctionPtr make_function(TObj * listener, bool (TObj::*method)(T&), int priority){
        return std::make_shared<Function>(priority, std::bind(method,listener,std::placeholders::_2), make_function_id(listener,method));
    }
    
    template<class TObj>
    FunctionPtr make_function(TObj * listener, void (TObj::*method)(T&), int priority){
        return std::make_shared<Function>(priority, [listener, method](const void*, T&t){
            ((listener)->*(method))(t);
            return false;
        }, make_function_id(listener,method));
    }
    
    template<class TObj>
    FunctionPtr make_function(TObj * listener, bool (TObj::*method)(const void*, T&), int priority){
        return std::make_shared<Function>(priority, std::bind(method,listener,std::placeholders::_1,std::placeholders::_2), make_function_id(listener,method));
    }
    
    template<class TObj>
    FunctionPtr make_function(TObj * listener, void (TObj::*method)(const void*, T&), int priority){
        return std::make_shared<Function>(priority, [listener, method](const void*s, T&t){
            std::bind(method,listener,std::placeholders::_1,std::placeholders::_2)(s,t);
            return false;
        }, make_function_id(listener,method));
    }
    
    FunctionPtr make_function(bool (*function)(T&), int priority){
        return std::make_shared<Function>(priority, std::bind(function,std::placeholders::_2), make_function_id((ofEvent<T>*)nullptr,function));
    }
    
    FunctionPtr make_function(void (*function)(T&), int priority){
        return std::make_shared<Function>(priority, [function](const void*, T&t){
            (function)(t);
            return false;
        }, make_function_id((ofEvent<T>*)nullptr,function));
    }
    
    FunctionPtr make_function(bool (*function)(const void*, T&), int priority){
        return std::make_shared<Function>(priority, function, make_function_id((ofEvent<T>*)nullptr,function));
    }
    
    FunctionPtr make_function(void (*function)(const void*, T&), int priority){
        return std::make_shared<Function>(priority, [function](const void*s, T&t){
            function(s,t);
            return false;
        }, make_function_id((ofEvent<T>*)nullptr,function));
    }
public:
    template<class TObj, typename TMethod>
    void add(TObj * listener, TMethod method, int priority){
        ofBaseEvent<of::priv::Function<T,Mutex>,Mutex>::add(make_function(listener,method,priority));
    }
    
    template<class TObj, typename TMethod>
    void remove(TObj * listener, TMethod method, int priority){
        ofBaseEvent<of::priv::Function<T,Mutex>,Mutex>::remove(make_function(listener,method,priority));
    }
    
    template<typename TFunction>
    void add(TFunction function, int priority){
        ofBaseEvent<of::priv::Function<T,Mutex>,Mutex>::add(make_function(function,priority));
    }
    
    template<typename TFunction>
    void remove(TFunction function, int priority){
        ofBaseEvent<of::priv::Function<T,Mutex>,Mutex>::remove(make_function(function,priority));
    }
    
    inline void notify(const void* sender, T & param){
        if(ofEvent<T,Mutex>::enabled && !ofEvent<T,Mutex>::functions.empty()){
            std::unique_lock<Mutex> lck(ofEvent<T,Mutex>::mtx);
            std::vector<std::shared_ptr<of::priv::Function<T,Mutex>>> functions_copy(ofEvent<T,Mutex>::functions);
            lck.unlock();
            for(auto & f: functions_copy){
                if(f->notify(sender,param)){
                    throw ofEventAttendedException();
                }
            }
        }
    }
};

template<typename Mutex>
class ofEvent<void,Mutex>: public ofBaseEvent<of::priv::Function<void,Mutex>,Mutex>{
protected:
    typedef of::priv::Function<void,Mutex> Function;
    typedef std::shared_ptr<Function> FunctionPtr;
    
    template<class TObj, typename TMethod>
    class FunctionId: public of::priv::BaseFunctionId{
    public:
        TObj * listener;
        TMethod method;
        
        FunctionId(TObj * listener, TMethod method)
        :listener(listener)
        ,method(method){
            
        }
        
        BaseFunctionId * clone() const{
            return new FunctionId<TObj,TMethod>(listener, method);
        }
        
        template<typename F>
        bool operator==(const F & f1) const{
            return f1.listener == this->listener && f1.method == this->method;
        }
        
        bool operator==(const BaseFunctionId & f) const{
            const auto * other = dynamic_cast<const FunctionId<TObj,TMethod>*>(&f);
            return other && other->listener == this->listener && other->method == this->method;
        }
    };
    
    
    template<class TObj, typename TMethod>
    std::unique_ptr<FunctionId<TObj,TMethod>> make_function_id(TObj * listener, TMethod method){
        return std::unique_ptr<FunctionId<TObj,TMethod>>(new FunctionId<TObj,TMethod>(listener,method));
    }
    
    template<class TObj>
    FunctionPtr make_function(TObj * listener, bool (TObj::*method)(), int priority){
        return std::make_shared<Function>(priority, std::bind(method,listener), make_function_id(listener,method));
    }
    
    template<class TObj>
    FunctionPtr make_function(TObj * listener, void (TObj::*method)(), int priority){
        return std::make_shared<Function>(priority,[listener, method](const void*){
            std::bind(method,listener)();
            return false;
        }, make_function_id(listener,method));
    }
    
    template<class TObj>
    FunctionPtr make_function(TObj * listener, bool (TObj::*method)(const void*), int priority){
        return std::make_shared<Function>(priority,std::bind(method,listener,std::placeholders::_1), make_function_id(listener,method));
    }
    
    template<class TObj>
    FunctionPtr make_function(TObj * listener, void (TObj::*method)(const void*), int priority){
        return std::make_shared<Function>(priority,[listener, method](const void* sender){
            std::bind(method,listener,std::placeholders::_1)(sender);
            return false;
        }, make_function_id(listener,method));
    }
    
    FunctionPtr make_function(bool (*function)(), int priority){
        return std::make_shared<Function>(priority, std::bind(function), make_function_id((ofEvent<void>*)nullptr,function));
    }
    
    FunctionPtr make_function(void (*function)(), int priority){
        return std::make_shared<Function>(priority,[function](const void*){
            function();
            return false;
        }, make_function_id((ofEvent<void>*)nullptr,function));
    }
    
    FunctionPtr make_function(bool (*function)(const void*), int priority){
        return std::make_shared<Function>(priority, function, make_function_id((ofEvent<void>*)nullptr,function));
    }
    
    FunctionPtr make_function(void (*function)(const void*), int priority){
        return std::make_shared<Function>(priority,[function](const void* sender){
            function(sender);
            return false;
        }, make_function_id((ofEvent<void>*)nullptr,function));
    }
public:
    template<class TObj, typename TMethod>
    void add(TObj * listener, TMethod method, int priority){
        ofBaseEvent<of::priv::Function<void,Mutex>,Mutex>::add(make_function(listener,method,priority));
    }
    
    template<class TObj, typename TMethod>
    void remove(TObj * listener, TMethod method, int priority){
        ofBaseEvent<of::priv::Function<void,Mutex>,Mutex>::remove(make_function(listener,method,priority));
    }
    
    template<typename TFunction>
    void add(TFunction function, int priority){
        ofBaseEvent<of::priv::Function<void,Mutex>,Mutex>::add(make_function(function,priority));
    }
    
    template<typename TFunction>
    void remove(TFunction function, int priority){
        ofBaseEvent<of::priv::Function<void,Mutex>,Mutex>::remove(make_function(function,priority));
    }
    
    void notify(const void* sender){
        if(ofEvent<void,Mutex>::enabled && !ofEvent<void,Mutex>::functions.empty()){
            std::unique_lock<Mutex> lck(ofEvent<void,Mutex>::mtx);
            std::vector<std::shared_ptr<of::priv::Function<void,Mutex>>> functions_copy(ofEvent<void,Mutex>::functions);
            lck.unlock();
            for(auto & f: functions_copy){
                if(f->notify(sender)){
                    throw ofEventAttendedException();
                }
            }
        }
    }
};


#ifndef _ofFastEvent_
#define _ofFastEvent_
/// Non thread safe event that avoids locks and copies of the listeners
/// making it faster than a plain ofEvent
template<typename T>
class ofFastEvent: public ofEvent<T,of::priv::NoopMutex>{
public:
    inline void notify(const void* sender, T & param){
        if(ofFastEvent::enabled){
            for(auto & f: ofFastEvent::functions){
                if(f->notify(sender,param)){
                    throw ofEventAttendedException();
                }
            }
        }
    }
};

#endif


#ifndef _OF_EVENT_UTILS_
#define _OF_EVENT_UTILS_

//----------------------------------------------------
// register any method of any class to an event.
// the method must provide one of the following
// signatures:
//     void method(ArgumentsType & args)
//     void method(const void * sender, ArgumentsType &args)
// ie:
//     ofAddListener(addon.newIntEvent, this, &Class::method)

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofAddListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
    event.add(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofAddListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
    event.add(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofAddListener(ofEvent<void> & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
    event.add(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofAddListener(ofEvent<void> & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
    event.add(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofAddListener(EventType & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
    event.add(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofAddListener(EventType & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
    event.add(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofAddListener(ofEvent<void> & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
    event.add(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofAddListener(ofEvent<void> & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
    event.add(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType>
void ofAddListener(EventType & event, void (*listenerFunction)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
    event.add(listenerFunction, prio);
}

template <class EventType,typename ArgumentsType>
void ofAddListener(EventType & event, void (*listenerFunction)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
    event.add(listenerFunction, prio);
}

inline void ofAddListener(ofEvent<void> & event, void (*listenerFunction)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
    event.add(listenerFunction, prio);
}

inline void ofAddListener(ofEvent<void> & event, void (*listenerFunction)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
    event.add(listenerFunction, prio);
}

template <class EventType,typename ArgumentsType>
void ofAddListener(EventType & event, bool (*listenerFunction)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
    event.add(listenerFunction, prio);
}

template <class EventType,typename ArgumentsType>
void ofAddListener(EventType & event, bool (*listenerFunction)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(*listenerFunction, prio);
    event.add(*listenerFunction, prio);
}

inline void ofAddListener(ofEvent<void> & event, bool (*listenerFunction)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(*listenerFunction, prio);
    event.add(*listenerFunction, prio);
}

inline void ofAddListener(ofEvent<void> & event, bool (*listenerFunction)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
    event.add(listenerFunction, prio);
}
//----------------------------------------------------
// unregister any method of any class to an event.
// the method must provide one the following
// signatures:
//     void method(ArgumentsType & args)
//     void method(const void * sender, ArgumentsType &args)
// ie:
//     ofAddListener(addon.newIntEvent, this, &Class::method)

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofRemoveListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofRemoveListener(EventType & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofRemoveListener(ofEvent<void> & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofRemoveListener(ofEvent<void> & event, ListenerClass  * listener, void (ListenerClass::*listenerMethod)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofRemoveListener(EventType & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType, class ListenerClass>
void ofRemoveListener(EventType & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofRemoveListener(ofEvent<void> & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
}

template <class ListenerClass>
void ofRemoveListener(ofEvent<void> & event, ListenerClass  * listener, bool (ListenerClass::*listenerMethod)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listener, listenerMethod, prio);
}

template <class EventType,typename ArgumentsType>
void ofRemoveListener(EventType & event, void (*listenerFunction)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
}

template <class EventType,typename ArgumentsType>
void ofRemoveListener(EventType & event, void (*listenerFunction)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
}

inline void ofRemoveListener(ofEvent<void> & event, void (*listenerFunction)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
}

inline void ofRemoveListener(ofEvent<void> & event, void (*listenerFunction)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
}

template <class EventType,typename ArgumentsType>
void ofRemoveListener(EventType & event, bool (*listenerFunction)(const void*, ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
}

template <class EventType,typename ArgumentsType>
void ofRemoveListener(EventType & event, bool (*listenerFunction)(ArgumentsType&), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
}

inline void ofRemoveListener(ofEvent<void> & event, bool (*listenerFunction)(const void*), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
}

inline void ofRemoveListener(ofEvent<void> & event, bool (*listenerFunction)(), int prio=OF_EVENT_ORDER_AFTER_APP){
    event.remove(listenerFunction, prio);
}
//----------------------------------------------------
// notifies an event so all the registered listeners
// get called
// ie:
//	ofNotifyEvent(addon.newIntEvent, intArgument, this)
//
// or in case there's no sender:
//	ofNotifyEvent(addon.newIntEvent, intArgument)

template <class EventType,typename ArgumentsType, typename SenderType>
inline void ofNotifyEvent(EventType & event, ArgumentsType & args, SenderType * sender){
    try{
        event.notify(sender,args);
    }catch(ofEventAttendedException &){
        
    }
}

template <class EventType,typename ArgumentsType>
inline void ofNotifyEvent(EventType & event, ArgumentsType & args){
    try{
        event.notify(nullptr,args);
    }catch(ofEventAttendedException &){
        
    }
}

template <class EventType, typename ArgumentsType, typename SenderType>
inline void ofNotifyEvent(EventType & event, const ArgumentsType & args, SenderType * sender){
    try{
        event.notify(sender,args);
    }catch(ofEventAttendedException &){
        
    }
}

template <class EventType,typename ArgumentsType>
inline void ofNotifyEvent(EventType & event, const ArgumentsType & args){
    try{
        event.notify(nullptr,args);
    }catch(ofEventAttendedException &){
        
    }
}

template <typename SenderType>
inline void ofNotifyEvent(ofEvent<void> & event, SenderType * sender){
    try{
        event.notify(sender);
    }catch(ofEventAttendedException &){
        
    }
}

inline void ofNotifyEvent(ofEvent<void> & event){
    try{
        event.notify(nullptr);
    }catch(ofEventAttendedException &){
        
    }
}


#endif


#endif // endif _OF_EVENTS_

#endif /* ofEvent_h */
