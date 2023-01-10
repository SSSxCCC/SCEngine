#include "core/Script.h"
#include "editor/UnknownScript.h"

namespace sc {

std::unordered_map<std::string, std::function<std::shared_ptr<Script>()>>* Script::sCreater;

std::shared_ptr<Script> Script::create(const ScriptData& data) {
    if (sCreater->contains(data.name)) {
        auto script = (*sCreater)[data.name]();
        script->setData(data);
        return script;
    } else { // name-not-found script
        auto script = std::make_shared<UnknownScript>();
        script->mUnknownScriptName = data.name;
        return script;
    }
}

} // namespace sc
