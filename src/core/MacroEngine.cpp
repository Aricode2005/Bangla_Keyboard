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
        // Only an object maps shortcuts to expansions. items() on an array would yield
        // "0", "1", ... as keys, turning typed digits into macros.
        if (!j.is_object()) {
            std::cerr << "[MacroEngine] Ignoring macros file: top level is not an object" << std::endl;
            return false;
        }
        MacroEngine loaded;
        for (auto& [key, value] : j.items()) {
            if (value.is_string()) {
                loaded.addMacro(key, value.get<std::string>());
            }
        }
        m_macros = std::move(loaded.m_macros);
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

bool MacroEngine::addMacro(const std::string& shortcut, const std::string& expansion) {
    const char* kSpace = " \t\r\n\f\v";
    const size_t first = shortcut.find_first_not_of(kSpace);
    if (first == std::string::npos || expansion.empty()) {
        return false;
    }
    const std::string trimmed = shortcut.substr(first, shortcut.find_last_not_of(kSpace) - first + 1);
    if (trimmed.find_first_of(kSpace) != std::string::npos) {
        return false;
    }
    m_macros[trimmed] = expansion;
    return true;
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
