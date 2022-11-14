#ifndef _UnknownScript_H_
#define _UnknownScript_H_

#include "core/Script.h"

// UnknownScript is created when we create a name-not-found script.
class UnknownScript : public Script {
public:
    std::string mUnknownScriptName;
    ScriptData getData() override {
        auto data = Script::getData();
        data.add("mUnknownScriptName", mUnknownScriptName);
        return data;
    }
    std::string getName() override { return "UnknownScript"; }
};
REGISTER_SCRIPT(UnknownScript)

#endif // _UnknownScript_H_
