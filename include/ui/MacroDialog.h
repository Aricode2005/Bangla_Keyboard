#pragma once
#include <windows.h>
#include <functional>
#include <string>
#include <vector>
#include "core/MacroEngine.h"

class MacroDialog {
public:
    MacroDialog(MacroEngine* engine);
    ~MacroDialog();

    void show(HINSTANCE instance);

    /// Called after every add or delete, so edits are persisted immediately rather than only
    /// when the application exits.
    void setOnChanged(std::function<void()> onChanged) { m_onChanged = std::move(onChanged); }

private:
    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    MacroEngine* m_engine;
    std::function<void()> m_onChanged;
    HWND m_hwnd = nullptr;
    HWND m_list = nullptr;
    HWND m_shortcutEdit = nullptr;
    HWND m_expansionEdit = nullptr;
    HWND m_addButton = nullptr;
    HWND m_deleteButton = nullptr;
    HFONT m_font = nullptr;
    HFONT m_bengaliFont = nullptr;

    /// Shortcut of each list row, by row index. Deleting looks the shortcut up here rather
    /// than parsing it back out of the displayed text.
    std::vector<std::string> m_rowShortcuts;

    void refreshList();
    void onAdd();
    void onDelete();
    void notifyChanged();
};
