#pragma once

#include "imgui.h"
#include "Snail/InputCodes.h"

namespace Snail {

    inline ImGuiKey SnailKey_To_ImGuiKey(int keycode)
    {
        switch (keycode)
        {
        case SNL_KEY_TAB: return ImGuiKey_Tab;
        case SNL_KEY_LEFT: return ImGuiKey_LeftArrow;
        case SNL_KEY_RIGHT: return ImGuiKey_RightArrow;
        case SNL_KEY_UP: return ImGuiKey_UpArrow;
        case SNL_KEY_DOWN: return ImGuiKey_DownArrow;
        case SNL_KEY_PAGE_UP: return ImGuiKey_PageUp;
        case SNL_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
        case SNL_KEY_HOME: return ImGuiKey_Home;
        case SNL_KEY_END: return ImGuiKey_End;
        case SNL_KEY_INSERT: return ImGuiKey_Insert;
        case SNL_KEY_DELETE: return ImGuiKey_Delete;
        case SNL_KEY_BACKSPACE: return ImGuiKey_Backspace;
        case SNL_KEY_SPACE: return ImGuiKey_Space;
        case SNL_KEY_ENTER: return ImGuiKey_Enter;
        case SNL_KEY_ESCAPE: return ImGuiKey_Escape;
        case SNL_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
        case SNL_KEY_COMMA: return ImGuiKey_Comma;
        case SNL_KEY_MINUS: return ImGuiKey_Minus;
        case SNL_KEY_PERIOD: return ImGuiKey_Period;
        case SNL_KEY_SLASH: return ImGuiKey_Slash;
        case SNL_KEY_SEMICOLON: return ImGuiKey_Semicolon;
        case SNL_KEY_EQUAL: return ImGuiKey_Equal;
        case SNL_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
        case SNL_KEY_BACKSLASH: return ImGuiKey_Backslash;
        case SNL_KEY_WORLD_1: return ImGuiKey_Oem102;
        case SNL_KEY_WORLD_2: return ImGuiKey_Oem102;
        case SNL_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
        case SNL_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
        case SNL_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
        case SNL_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
        case SNL_KEY_NUM_LOCK: return ImGuiKey_NumLock;
        case SNL_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
        case SNL_KEY_PAUSE: return ImGuiKey_Pause;
        case SNL_KEY_KP_0: return ImGuiKey_Keypad0;
        case SNL_KEY_KP_1: return ImGuiKey_Keypad1;
        case SNL_KEY_KP_2: return ImGuiKey_Keypad2;
        case SNL_KEY_KP_3: return ImGuiKey_Keypad3;
        case SNL_KEY_KP_4: return ImGuiKey_Keypad4;
        case SNL_KEY_KP_5: return ImGuiKey_Keypad5;
        case SNL_KEY_KP_6: return ImGuiKey_Keypad6;
        case SNL_KEY_KP_7: return ImGuiKey_Keypad7;
        case SNL_KEY_KP_8: return ImGuiKey_Keypad8;
        case SNL_KEY_KP_9: return ImGuiKey_Keypad9;
        case SNL_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
        case SNL_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
        case SNL_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
        case SNL_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
        case SNL_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
        case SNL_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
        case SNL_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
        case SNL_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
        case SNL_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
        case SNL_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
        case SNL_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
        case SNL_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
        case SNL_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
        case SNL_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
        case SNL_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
        case SNL_KEY_MENU: return ImGuiKey_Menu;
        case SNL_KEY_0: return ImGuiKey_0;
        case SNL_KEY_1: return ImGuiKey_1;
        case SNL_KEY_2: return ImGuiKey_2;
        case SNL_KEY_3: return ImGuiKey_3;
        case SNL_KEY_4: return ImGuiKey_4;
        case SNL_KEY_5: return ImGuiKey_5;
        case SNL_KEY_6: return ImGuiKey_6;
        case SNL_KEY_7: return ImGuiKey_7;
        case SNL_KEY_8: return ImGuiKey_8;
        case SNL_KEY_9: return ImGuiKey_9;
        case SNL_KEY_A: return ImGuiKey_A;
        case SNL_KEY_B: return ImGuiKey_B;
        case SNL_KEY_C: return ImGuiKey_C;
        case SNL_KEY_D: return ImGuiKey_D;
        case SNL_KEY_E: return ImGuiKey_E;
        case SNL_KEY_F: return ImGuiKey_F;
        case SNL_KEY_G: return ImGuiKey_G;
        case SNL_KEY_H: return ImGuiKey_H;
        case SNL_KEY_I: return ImGuiKey_I;
        case SNL_KEY_J: return ImGuiKey_J;
        case SNL_KEY_K: return ImGuiKey_K;
        case SNL_KEY_L: return ImGuiKey_L;
        case SNL_KEY_M: return ImGuiKey_M;
        case SNL_KEY_N: return ImGuiKey_N;
        case SNL_KEY_O: return ImGuiKey_O;
        case SNL_KEY_P: return ImGuiKey_P;
        case SNL_KEY_Q: return ImGuiKey_Q;
        case SNL_KEY_R: return ImGuiKey_R;
        case SNL_KEY_S: return ImGuiKey_S;
        case SNL_KEY_T: return ImGuiKey_T;
        case SNL_KEY_U: return ImGuiKey_U;
        case SNL_KEY_V: return ImGuiKey_V;
        case SNL_KEY_W: return ImGuiKey_W;
        case SNL_KEY_X: return ImGuiKey_X;
        case SNL_KEY_Y: return ImGuiKey_Y;
        case SNL_KEY_Z: return ImGuiKey_Z;
        case SNL_KEY_F1: return ImGuiKey_F1;
        case SNL_KEY_F2: return ImGuiKey_F2;
        case SNL_KEY_F3: return ImGuiKey_F3;
        case SNL_KEY_F4: return ImGuiKey_F4;
        case SNL_KEY_F5: return ImGuiKey_F5;
        case SNL_KEY_F6: return ImGuiKey_F6;
        case SNL_KEY_F7: return ImGuiKey_F7;
        case SNL_KEY_F8: return ImGuiKey_F8;
        case SNL_KEY_F9: return ImGuiKey_F9;
        case SNL_KEY_F10: return ImGuiKey_F10;
        case SNL_KEY_F11: return ImGuiKey_F11;
        case SNL_KEY_F12: return ImGuiKey_F12;
        case SNL_KEY_F13: return ImGuiKey_F13;
        case SNL_KEY_F14: return ImGuiKey_F14;
        case SNL_KEY_F15: return ImGuiKey_F15;
        case SNL_KEY_F16: return ImGuiKey_F16;
        case SNL_KEY_F17: return ImGuiKey_F17;
        case SNL_KEY_F18: return ImGuiKey_F18;
        case SNL_KEY_F19: return ImGuiKey_F19;
        case SNL_KEY_F20: return ImGuiKey_F20;
        case SNL_KEY_F21: return ImGuiKey_F21;
        case SNL_KEY_F22: return ImGuiKey_F22;
        case SNL_KEY_F23: return ImGuiKey_F23;
        case SNL_KEY_F24: return ImGuiKey_F24;
        default: return ImGuiKey_None;
        }
    }

}