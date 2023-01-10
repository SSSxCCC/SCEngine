#ifndef _UnknownScript_H_
#define _UnknownScript_H_

#include "core/Script.h"

namespace sc {

// UnknownScript is created when we create a name-not-found script.
class UnknownScript : public Script {
public:
    SCRIPT_BODY(UnknownScript)
    std::string mUnknownScriptName;
    ScriptData getData() override {
        auto data = Script::getData();
        data.add("mUnknownScriptName", mUnknownScriptName);
        return data;
    }
};
REGISTER_SCRIPT(UnknownScript)

} // namespace sc

#endif // _UnknownScript_H_
