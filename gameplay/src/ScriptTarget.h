#ifndef SCRIPTTARGET_H_
#define SCRIPTTARGET_H_

#include "Base.h"

namespace gameplay
{

/**
 * Generic base class for supporting script callbacks.
 * 
 * @script{ignore}
 */
class ScriptTarget
{
public:
    /**
     * Destructor.
     */
    virtual ~ScriptTarget();

    /**
     * Fires the event with the given event name and the given arguments.
     * 
     * @param eventName The name of the event.
     */
    template<typename T> T fireEvent(const char* eventName, ...);

    /**
     * Adds the given Lua script function as a callback for the given event.
     * 
     * @param eventName The name of the event.
     * @param function The name of the Lua script function to call when the event is fired; can either be
     *      just the name of a function (if the function's script file has already been loaded), or can be
     *      a URL of the form scriptFile.lua#functionName.
     * @param data Optional data for the callback.
     */
    virtual void addCallback(const std::string& eventName, const std::string& function, void* data = NULL);

    /**
     * Removes the given Lua script function as a callback for the given event.
     * 
     * @param eventName The name of the event.
     * @param function The name of the Lua script function.
     * @param data Optional data for the callback.
     */
    virtual void removeCallback(const std::string& eventName, const std::string& function, void* data = NULL);

protected:
    /**
     * Adds the given event with the given Lua script parameter string ({@link ScriptController::executeFunction})
     * as a supported event for this script target.
     * 
     * @param eventName The name of the event.
     * @param argsString The argument string for the event.
     */
    void addEvent(const std::string& eventName, const char* argsString = NULL);

    /** Used to store a script callbacks for given event. */
    struct Callback
    {
        /** Constructor. */
        Callback(const std::string& string, void* data);

        /** Holds the Lua script callback function. */
        std::string function;
        /** Generic data storage. */
        void* data;
    };

    /** Holds the supported events for this script target. */
    std::map<std::string, std::string> _events;
    /** Holds the callbacks for this script target's events. */
    std::map<std::string, std::vector<Callback>*> _callbacks;
};

template<typename T> T ScriptTarget::fireEvent(const char* eventName, ...)
{
    GP_ERROR("Unsupported return type!");
}

/** Template specialization. */
template<> void ScriptTarget::fireEvent<void>(const char* eventName, ...);
/** Template specialization. */
template<> bool ScriptTarget::fireEvent<bool>(const char* eventName, ...);

}

#endif
