#include "core/MacroEngine.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>

using json = nlohmann::json;

std::string MacroEngine::defaultPath() {
#ifdef _WIN32
    if (const char* appdata = std::getenv("APPDATA")) {
        return std::string(appdata) + "\\Shobdomala\\macros.json";
    }
#endif
    return "shobdomala_macros.json";
}

bool MacroEngine::load(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    try {
        json j;
        file >> j;
        m_macros.clear();
        for (auto& [key, value] : j.items()) {
            if (value.is_string()) {
                m_macros[key] = value.get<std::string>();
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[MacroEngine] Failed to parse macros: " << e.what() << std::endl;
        return false;
    }
}

bool MacroEngine::save(const std::string& path) const {
    json j = m_macros;
    std::ofstream file(path, std::ios::trunc | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file << j.dump(4);
    return file.good();
}

void MacroEngine::addMacro(const std::string& shortcut, const std::string& expansion) {
    if (!shortcut.empty()) {
        m_macros[shortcut] = expansion;
    }
}

bool MacroEngine::removeMacro(const std::string& shortcut) {
    return m_macros.erase(shortcut) > 0;
}

std::string MacroEngine::expand(const std::string& input) const {
    auto it = m_macros.find(input);
    if (it != m_macros.end()) {
        return it->second;
    }
    return "";
}

bool MacroEngine::hasMacro(const std::string& input) const {
    return m_macros.find(input) != m_macros.end();
}
