#include "Script.h"

std::unordered_map<std::string, std::function<std::shared_ptr<Script>()>>* Script::sCreater;
