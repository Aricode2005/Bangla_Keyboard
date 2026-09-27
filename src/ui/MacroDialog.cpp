#include "ui/MacroDialog.h"
#include "ui/UiTheme.h"
#include <string>
#include <vector>

namespace {
    const wchar_t* kClassName = L"ShobdomalaMacroWindow";
    
    // IDs for child controls
    constexpr int ID_LIST = 101;
    constexpr int ID_SHORTCUT_EDIT = 102;
    constexpr int ID_EXPANSION_EDIT = 103;
    constexpr int ID_ADD_BUTTON = 104;
    constexpr int ID_DELETE_BUTTON = 105;
}

MacroDialog::MacroDialog(MacroEngine* engine) : m_engine(engine) {}

MacroDialog::~MacroDialog() {
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
}

void MacroDialog::show(HINSTANCE instance) {
    if (m_hwnd) {
        SetForegroundWindow(m_hwnd);
        return;
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &MacroDialog::wndProc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = kClassName;
    RegisterClassExW(&wc);

    m_hwnd = CreateWindowExW(
        0, kClassName, L"Edit Macros - Shobdomala",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 350,
        nullptr, nullptr, instance, this
    );

    if (!m_hwnd) return;

    HFONT font = UiTheme::createUiFont(16, 96, FW_NORMAL);

    CreateWindowExW(0, L"STATIC", L"Macros (Shortcut -> Expansion):", WS_CHILD | WS_VISIBLE,
                    10, 10, 200, 20, m_hwnd, nullptr, instance, nullptr);

    m_list = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", nullptr,
                             WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY,
                             10, 30, 360, 200, m_hwnd, (HMENU)ID_LIST, instance, nullptr);
    SendMessage(m_list, WM_SETFONT, (WPARAM)font, TRUE);

    CreateWindowExW(0, L"STATIC", L"Shortcut:", WS_CHILD | WS_VISIBLE,
                    10, 240, 60, 20, m_hwnd, nullptr, instance, nullptr);
    m_shortcutEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", nullptr,
                                     WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                     70, 240, 100, 20, m_hwnd, (HMENU)ID_SHORTCUT_EDIT, instance, nullptr);
    SendMessage(m_shortcutEdit, WM_SETFONT, (WPARAM)font, TRUE);

    CreateWindowExW(0, L"STATIC", L"Expansion:", WS_CHILD | WS_VISIBLE,
                    180, 240, 70, 20, m_hwnd, nullptr, instance, nullptr);
    m_expansionEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", nullptr,
                                      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                      255, 240, 115, 20, m_hwnd, (HMENU)ID_EXPANSION_EDIT, instance, nullptr);
    
    HFONT bengaliFont = UiTheme::createBengaliFont(16, 96, FW_NORMAL);
    SendMessage(m_expansionEdit, WM_SETFONT, (WPARAM)bengaliFont, TRUE);

    m_addButton = CreateWindowExW(0, L"BUTTON", L"Add",
                                  WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                  200, 275, 80, 25, m_hwnd, (HMENU)ID_ADD_BUTTON, instance, nullptr);
    SendMessage(m_addButton, WM_SETFONT, (WPARAM)font, TRUE);

    m_deleteButton = CreateWindowExW(0, L"BUTTON", L"Delete",
                                     WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                     290, 275, 80, 25, m_hwnd, (HMENU)ID_DELETE_BUTTON, instance, nullptr);
    SendMessage(m_deleteButton, WM_SETFONT, (WPARAM)font, TRUE);

    refreshList();
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
}

void MacroDialog::refreshList() {
    SendMessageW(m_list, LB_RESETCONTENT, 0, 0);
    if (!m_engine) return;

    for (const auto& [shortcut, expansion] : m_engine->getMacros()) {
        std::string display = shortcut + " -> " + expansion;
        int wlen = MultiByteToWideChar(CP_UTF8, 0, display.c_str(), -1, nullptr, 0);
        std::wstring wdisplay(wlen, 0);
        MultiByteToWideChar(CP_UTF8, 0, display.c_str(), -1, wdisplay.data(), wlen);
        SendMessageW(m_list, LB_ADDSTRING, 0, (LPARAM)wdisplay.c_str());
    }
}

void MacroDialog::onAdd() {
    wchar_t wShortcut[256] = {0};
    wchar_t wExpansion[256] = {0};
    GetWindowTextW(m_shortcutEdit, wShortcut, 256);
    GetWindowTextW(m_expansionEdit, wExpansion, 256);

    std::string shortcut, expansion;
    
    int len1 = WideCharToMultiByte(CP_UTF8, 0, wShortcut, -1, nullptr, 0, nullptr, nullptr);
    if (len1 > 1) { shortcut.resize(len1 - 1); WideCharToMultiByte(CP_UTF8, 0, wShortcut, -1, &shortcut[0], len1, nullptr, nullptr); }

    int len2 = WideCharToMultiByte(CP_UTF8, 0, wExpansion, -1, nullptr, 0, nullptr, nullptr);
    if (len2 > 1) { expansion.resize(len2 - 1); WideCharToMultiByte(CP_UTF8, 0, wExpansion, -1, &expansion[0], len2, nullptr, nullptr); }

    if (!shortcut.empty() && !expansion.empty()) {
        m_engine->addMacro(shortcut, expansion);
        refreshList();
        SetWindowTextW(m_shortcutEdit, L"");
        SetWindowTextW(m_expansionEdit, L"");
    }
}

void MacroDialog::onDelete() {
    LRESULT sel = SendMessageW(m_list, LB_GETCURSEL, 0, 0);
    if (sel == LB_ERR) return;

    wchar_t text[512] = {0};
    SendMessageW(m_list, LB_GETTEXT, sel, (LPARAM)text);
    
    std::wstring wtext(text);
    size_t arrow = wtext.find(L" ->");
    if (arrow != std::wstring::npos) {
        std::wstring wShortcut = wtext.substr(0, arrow);
        std::string shortcut;
        int len = WideCharToMultiByte(CP_UTF8, 0, wShortcut.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (len > 1) { shortcut.resize(len - 1); WideCharToMultiByte(CP_UTF8, 0, wShortcut.c_str(), -1, &shortcut[0], len, nullptr, nullptr); }
        
        m_engine->removeMacro(shortcut);
        refreshList();
    }
}

LRESULT CALLBACK MacroDialog::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MacroDialog* self = reinterpret_cast<MacroDialog*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    if (msg == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    if (!self) return DefWindowProcW(hwnd, msg, wParam, lParam);

    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_ADD_BUTTON) {
                self->onAdd();
            } else if (LOWORD(wParam) == ID_DELETE_BUTTON) {
                self->onDelete();
            }
            return 0;
        case WM_DESTROY:
            self->m_hwnd = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
