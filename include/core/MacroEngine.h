#pragma once
#include <string>
#include <unordered_map>

class MacroEngine {
public:
    static std::string defaultPath();

    bool load(const std::string& path);
    bool save(const std::string& path) const;

    void addMacro(const std::string& shortcut, const std::string& expansion);
    bool removeMacro(const std::string& shortcut);
    
    // Returns the expanded string if it exists, otherwise empty string
    std::string expand(const std::string& input) const;
    
    bool hasMacro(const std::string& input) const;

    const std::unordered_map<std::string, std::string>& getMacros() const { return m_macros; }

private:
    std::unordered_map<std::string, std::string> m_macros;
};
