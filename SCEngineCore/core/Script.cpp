#include "Script.h"

std::unordered_map<std::string, std::function<std::shared_ptr<Script>()>>* Script::sCreater;

std::shared_ptr<Script> Script::create(const ScriptData& data) {
	auto script = (*sCreater)[data.name]();
	script->setData(data);
	return script;
}
