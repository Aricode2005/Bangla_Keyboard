#pragma once
#include <map>
#include <string>

class MacroEngine {
public:
    static std::string defaultPath();

    /// Replaces the current macros with the file's. A missing, malformed or non-object file
    /// leaves the current macros untouched and returns false; invalid entries are skipped.
    bool load(const std::string& path);
    bool save(const std::string& path) const;

    /// Surrounding whitespace is trimmed from the shortcut. Returns false, and changes
    /// nothing, when the shortcut is empty or contains whitespace (the typing buffer ends at
    /// a delimiter, so such a shortcut could never be typed) or the expansion is empty (it
    /// would silently delete the word).
    bool addMacro(const std::string& shortcut, const std::string& expansion);
    bool removeMacro(const std::string& shortcut);

    // Returns the expanded string if it exists, otherwise empty string
    std::string expand(const std::string& input) const;

    bool hasMacro(const std::string& input) const;

    /// Ordered, so the dialog lists and the file stores macros in a stable order.
    const std::map<std::string, std::string>& getMacros() const { return m_macros; }

private:
    std::map<std::string, std::string> m_macros;
};
