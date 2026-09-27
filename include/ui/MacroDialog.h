#pragma once
#include <windows.h>
#include "core/MacroEngine.h"

class MacroDialog {
public:
    MacroDialog(MacroEngine* engine);
    ~MacroDialog();

    void show(HINSTANCE instance);

private:
    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    MacroEngine* m_engine;
    HWND m_hwnd = nullptr;
    HWND m_list = nullptr;
    HWND m_shortcutEdit = nullptr;
    HWND m_expansionEdit = nullptr;
    HWND m_addButton = nullptr;
    HWND m_deleteButton = nullptr;

    void refreshList();
    void onAdd();
    void onDelete();
};
