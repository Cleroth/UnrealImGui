#pragma once

// The MIT License (MIT)
//
// Copyright (c) 2021 Frank D. Martinez M. <aka mnesarco>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "imgui.h"

// clang-format off

// ----------------------------------------------------------------------------
// [SECTION] RAII guard implementations
// ----------------------------------------------------------------------------

namespace ImGuiSugar
{
    using ScopeEndCallback = void(*)();

    // RAII scope guard for ImGui Begin* functions returning bool.
    template<bool AlwaysCallEnd>
    struct BooleanGuard
    {
        BooleanGuard(const bool state, const ScopeEndCallback end) noexcept
            : m_state(state), m_end(end) {}

        BooleanGuard(const BooleanGuard&) = delete;
        BooleanGuard(BooleanGuard&&) = delete;
        BooleanGuard& operator=(const BooleanGuard&) = delete; // NOLINT
        BooleanGuard& operator=(BooleanGuard&&) = delete; // NOLINT

        ~BooleanGuard() noexcept 
        {
            if (AlwaysCallEnd || m_state) { m_end(); }
        }

        operator bool() const & noexcept { return m_state; } // (Implicit) NOLINT

        private:
            const bool m_state;
            const ScopeEndCallback m_end;
    };

    // For special cases, transform void(*)(int) to void(*)()
    inline void PopStyleColor() { ImGui::PopStyleColor(1); };
    inline void PopStyleVar()   { ImGui::PopStyleVar(1); };

    // Tooltip auto triggered on hover
    inline auto BeginTooltip() -> bool
    {
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            return true;
        }
        return false;
    }

} // namespace ImGuiSugar

// ----------------------------------------------------------------------------
// [SECTION] Utility macros
// ----------------------------------------------------------------------------

// Portable Expression Statement, calls void function and returns true
#define IMGUI_SUGAR_ES(FN, ...) ([&]() -> bool { FN(__VA_ARGS__); return true; }())
#define IMGUI_SUGAR_ES_0(FN) ([&]() -> bool { FN(); return true; }())

// Concatenating symbols with __LINE__ requires two levels of indirection
#define IMGUI_SUGAR_CONCAT0(A, B) A ## B
#define IMGUI_SUGAR_CONCAT1(A, B) IMGUI_SUGAR_CONCAT0(A, B)

// ----------------------------------------------------------------------------
// [SECTION] Generic macros to simplify repetitive declarations
// ----------------------------------------------------------------------------
// 
// +----------------------+-------------------+-----------------+---------------------+
// | BEGIN                | END               | ALWAYS          | __VA_ARGS__         |
// +----------------------+-------------------+-----------------+---------------------+
// | Begin*/Push*         | End*/Pop*         | Is call to END  | Begin*/Push*        |
// | function name        | function name     | unconditional?  | function arguments  |
// +----------------------+-------------------+-----------------+---------------------+

#define IMGUI_SUGAR_SCOPED_BOOL(BEGIN, END, ALWAYS, ...) \
    if (const ImGuiSugar::BooleanGuard<ALWAYS> IMGUI_SUGAR_CONCAT1(_ui_scope_guard, __LINE__) = {BEGIN(__VA_ARGS__), &END})

#define IMGUI_SUGAR_SCOPED_BOOL_0(BEGIN, END, ALWAYS) \
    if (const ImGuiSugar::BooleanGuard<ALWAYS> IMGUI_SUGAR_CONCAT1(_ui_scope_guard, __LINE__) = {BEGIN(), &END})

#define IMGUI_SUGAR_SCOPED_VOID_N(BEGIN, END, ...) \
    if (const ImGuiSugar::BooleanGuard<true> IMGUI_SUGAR_CONCAT1(_ui_scope_guard, __LINE__) = {IMGUI_SUGAR_ES(BEGIN, __VA_ARGS__), &END})

#define IMGUI_SUGAR_SCOPED_VOID_0(BEGIN, END) \
    if (const ImGuiSugar::BooleanGuard<true> IMGUI_SUGAR_CONCAT1(_ui_scope_guard, __LINE__) = {IMGUI_SUGAR_ES_0(BEGIN), &END})

#define IMGUI_SUGAR_PARENT_SCOPED_VOID_N(BEGIN, END, ...) \
    const ImGuiSugar::BooleanGuard<true> IMGUI_SUGAR_CONCAT1(_ui_scope_, __LINE__) = {IMGUI_SUGAR_ES(BEGIN, __VA_ARGS__), &END}

// ---------------------------------------------------------------------------
// [SECTION] ImGui DSL
// ----------------------------------------------------------------------------

// Self scoped blocks

// const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0
#define with_Window(...)             IMGUI_SUGAR_SCOPED_BOOL(ImGui::Begin,                   ImGui::End,               true,  __VA_ARGS__)
// const char* str_id, ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0
#define with_Child(...)              IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginChild,              ImGui::EndChild,          true,  __VA_ARGS__)
// ImGuiID id, ImVec2& size, ImGuiWindowFlags flags = 0
#define with_ChildFrame(...)         IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginChildFrame,         ImGui::EndChildFrame,     true,  __VA_ARGS__)
// const char* label, const char* preview_value, ImGuiComboFlags flags = 0
#define with_Combo(...)              IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginCombo,              ImGui::EndCombo,          false, __VA_ARGS__)
// const char* label, const ImVec2& size = ImVec2(0, 0)
#define with_ListBox(...)            IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginListBox,            ImGui::EndListBox,        false, __VA_ARGS__)
// const char* label, bool enabled = true
#define with_Menu(...)               IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginMenu,               ImGui::EndMenu,           false, __VA_ARGS__)
// const char* str_id, ImGuiWindowFlags flags = 0
#define with_Popup(...)              IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopup,              ImGui::EndPopup,          false, __VA_ARGS__)
// const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0
#define with_PopupModal(...)         IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopupModal,         ImGui::EndPopup,          false, __VA_ARGS__)
// const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1
#define with_PopupContextItem(...)   IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopupContextItem,   ImGui::EndPopup,          false, __VA_ARGS__)
// const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1
#define with_PopupContextWindow(...) IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopupContextWindow, ImGui::EndPopup,          false, __VA_ARGS__)
// const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1
#define with_PopupContextVoid(...)   IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginPopupContextVoid,   ImGui::EndPopup,          false, __VA_ARGS__)
// const char* str_id, int column, ImGuiTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f
#define with_Table(...)              IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginTable,              ImGui::EndTable,          false, __VA_ARGS__)
// const char* str_id, ImGuiTabBarFlags flags = 0
#define with_TabBar(...)             IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginTabBar,             ImGui::EndTabBar,         false, __VA_ARGS__)
// const char* label, bool* p_open = NULL, ImGuiTabItemFlags flags = 0
#define with_TabItem(...)            IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginTabItem,            ImGui::EndTabItem,        false, __VA_ARGS__)
// ImGuiDragDropFlags flags = 0
#define with_DragDropSource(...)     IMGUI_SUGAR_SCOPED_BOOL(ImGui::BeginDragDropSource,     ImGui::EndDragDropSource, false, __VA_ARGS__)
#define with_TreeNode(...)           IMGUI_SUGAR_SCOPED_BOOL(ImGui::TreeNode,                ImGui::TreePop,           false, __VA_ARGS__)
#define with_TreeNodeV(...)          IMGUI_SUGAR_SCOPED_BOOL(ImGui::TreeNodeV,               ImGui::TreePop,           false, __VA_ARGS__)
#define with_TreeNodeEx(...)         IMGUI_SUGAR_SCOPED_BOOL(ImGui::TreeNodeEx,              ImGui::TreePop,           false, __VA_ARGS__)
#define with_TreeNodeExV(...)        IMGUI_SUGAR_SCOPED_BOOL(ImGui::TreeNodeExV,             ImGui::TreePop,           false, __VA_ARGS__)

#define with_TooltipOnHover()        IMGUI_SUGAR_SCOPED_BOOL_0(ImGuiSugar::BeginTooltip,     ImGui::EndTooltip,        false)
#define with_DragDropTarget()        IMGUI_SUGAR_SCOPED_BOOL_0(ImGui::BeginDragDropTarget,   ImGui::EndDragDropTarget, false)
#define with_MainMenuBar()           IMGUI_SUGAR_SCOPED_BOOL_0(ImGui::BeginMainMenuBar,      ImGui::EndMainMenuBar,    false)
#define with_MenuBar()               IMGUI_SUGAR_SCOPED_BOOL_0(ImGui::BeginMenuBar,          ImGui::EndMenuBar,        false)

#define with_Group()                 IMGUI_SUGAR_SCOPED_VOID_0(ImGui::BeginGroup,            ImGui::EndGroup)
#define with_Tooltip()               IMGUI_SUGAR_SCOPED_VOID_0(ImGui::BeginTooltip,          ImGui::EndTooltip)

// ImFont* font
#define with_Font(...)               IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushFont,               ImGui::PopFont,               __VA_ARGS__)
// bool repeat
#define with_ButtonRepeat(...)       IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushButtonRepeat,       ImGui::PopButtonRepeat,       __VA_ARGS__)
// float item_width
#define with_ItemWidth(...)          IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushItemWidth,          ImGui::PopItemWidth,          __VA_ARGS__)
// float wrap_pos_x
#define with_TextWrapPos(...)        IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushTextWrapPos,        ImGui::PopTextWrapPos,        __VA_ARGS__)
// const char* str_id
#define with_ID(...)                 IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushID,                 ImGui::PopID,                 __VA_ARGS__)
// const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect = false
#define with_ClipRect(...)           IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushClipRect,           ImGui::PopClipRect,           __VA_ARGS__)
// ImTextureID texture_id
#define with_TextureID(...)          IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushTextureID,          ImGui::PopTextureID,          __VA_ARGS__)

// Non self scoped guards (managed by parent scope)

#define set_Font(...)                IMGUI_SUGAR_PARENT_SCOPED_VOID_N(ImGui::PushFont,               ImGui::PopFont,               __VA_ARGS__)
#define set_ButtonRepeat(...)        IMGUI_SUGAR_PARENT_SCOPED_VOID_N(ImGui::PushButtonRepeat,       ImGui::PopButtonRepeat,       __VA_ARGS__)
#define set_ItemWidth(...)           IMGUI_SUGAR_PARENT_SCOPED_VOID_N(ImGui::PushItemWidth,          ImGui::PopItemWidth,          __VA_ARGS__)
#define set_TextWrapPos(...)         IMGUI_SUGAR_PARENT_SCOPED_VOID_N(ImGui::PushTextWrapPos,        ImGui::PopTextWrapPos,        __VA_ARGS__)
#define set_ID(...)                  IMGUI_SUGAR_PARENT_SCOPED_VOID_N(ImGui::PushID,                 ImGui::PopID,                 __VA_ARGS__)
#define set_ClipRect(...)            IMGUI_SUGAR_PARENT_SCOPED_VOID_N(ImGui::PushClipRect,           ImGui::PopClipRect,           __VA_ARGS__)
#define set_TextureID(...)           IMGUI_SUGAR_PARENT_SCOPED_VOID_N(ImGui::PushTextureID,          ImGui::PopTextureID,          __VA_ARGS__)

// Special case (overloaded functions StyleColor and StyleVar)

// ImGuiCol idx, const ImVec4& col
#define set_StyleColor(...)          IMGUI_SUGAR_PARENT_SCOPED_VOID_N(ImGui::PushStyleColor,  ImGuiSugar::PopStyleColor,           __VA_ARGS__)
// ImGuiStyleVar idx, [float val / const ImVec2& val]
#define set_StyleVar(...)            IMGUI_SUGAR_PARENT_SCOPED_VOID_N(ImGui::PushStyleVar,    ImGuiSugar::PopStyleVar,             __VA_ARGS__)

// ImGuiCol idx, const ImVec4& col
#define with_StyleColor(...)         IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushStyleColor,         ImGuiSugar::PopStyleColor,           __VA_ARGS__)
// ImGuiStyleVar idx, [float val / const ImVec2& val]
#define with_StyleVar(...)           IMGUI_SUGAR_SCOPED_VOID_N(ImGui::PushStyleVar,           ImGuiSugar::PopStyleVar,             __VA_ARGS__)

// Non RAII 

// const char* label, [opt] bool* p_visible, ImGuiTreeNodeFlags flags = 0
#define with_CollapsingHeader(...) if (ImGui::CollapsingHeader(__VA_ARGS__))
#define with_MenuItem(...) if (ImGui::MenuItem(__VA_ARGS__))

namespace ImGui {
IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0,
                          ImGuiInputTextCallback callback = nullptr, void* user_data = nullptr);
}

/*******  ▞▀▖▐     ▜      ▌    ▐     ▗▀▖▗▀▖ ******************/
/*******  ▚▄ ▜▀ ▌ ▌▐ ▞▀▖▞▀▌ ▞▀▘▜▀ ▌ ▌▐  ▐   ******************/
/*******  ▖ ▌▐ ▖▚▄▌▐ ▛▀ ▌ ▌ ▝▀▖▐ ▖▌ ▌▜▀ ▜▀  ******************/
/*******  ▝▀  ▀ ▗▄▘ ▘▝▀▘▝▀▘ ▀▀  ▀ ▝▀▘▐  ▐ ******************/

#define with_ButtonColored(r,g,b)  \
	if(const ImGuiSugar::BooleanGuard<true> IMGUI_SUGAR_CONCAT1(_ui_scope_guard, __LINE__) \
		= {[]{ \
			PushStyleColor(ImGuiCol_Button       , {r,g,b,0.5}); \
			PushStyleColor(ImGuiCol_ButtonHovered, {r,g,b,0.8}); \
			PushStyleColor(ImGuiCol_ButtonActive , {r,g,b,0.7}); \
			return true; \
		}(), []{ \
			PopStyleColor(3); \
		}})

#define ComboEnum(label, var)  \
	with_Combo(label, enum_name(var).data(), ImGuiComboFlags_HeightLarge)		\
	{																					\
		for(auto [val, name] : enum_entries<decltype(var)>())	\
		{																				\
			if(Selectable(name.data(), var == val))		\
				var = val;											\
		}																				\
	}

#define InputDuration(label, var)	SetNextItemWidth(CalcTextSize(label).x + 70), \
												InputFloat(label, var, 0.1, 1, "%g")
#define InputAngle(label, var)		\
	SetNextItemWidth(CalcTextSize(label).x + 70), \
	InputFloat(label, var, 5, 25, "%g"), \
	*(var) = FMath::Clamp(*(var), 0, 180)

// clang-format on
