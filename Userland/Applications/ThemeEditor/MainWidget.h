/*
 * Copyright (c) 2022, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "PreviewWidget.h"
#include <AK/FixedArray.h>
#include <AK/Time.h>
#include <LibGUI/CheckBox.h>
#include <LibGUI/ColorInput.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/SpinBox.h>
#include <LibGUI/TabWidget.h>
#include <LibGUI/TextBox.h>
#include <LibGUI/Window.h>
#include <LibGfx/SystemTheme.h>

namespace ThemeEditor {

struct Property {
    Variant<Gfx::AlignmentRole, Gfx::ColorRole, Gfx::FlagRole, Gfx::MetricRole, Gfx::PathRole> role;
};

struct PropertyTab {
    String title;
    Vector<Property> properties;
};

class MainWidget final : public GUI::Widget {
    C_OBJECT(MainWidget);

public:
    virtual ~MainWidget() override = default;

    ErrorOr<void> initialize_menubar(GUI::Window&);
    GUI::Window::CloseRequestDecision request_close();
    void update_title();

private:
    explicit MainWidget(Optional<String> path, Gfx::Palette startup_preview_palette);

    void load_from_file(String const& path);
    void save_to_file(Core::File&);
    void set_path(String);

    void add_property_tab(PropertyTab&);
    void set_alignment(Gfx::AlignmentRole, Gfx::TextAlignment);
    void set_color(Gfx::ColorRole, Gfx::Color);
    void set_flag(Gfx::FlagRole, bool);
    void set_metric(Gfx::MetricRole, int);
    void set_path(Gfx::PathRole, String);

    enum class PathPickerTarget {
        File,
        Folder,
    };
    void show_path_picker_dialog(StringView property_display_name, GUI::TextBox&, PathPickerTarget);

    RefPtr<PreviewWidget> m_preview_widget;
    RefPtr<GUI::TabWidget> m_property_tabs;
    RefPtr<GUI::Action> m_save_action;

    Optional<String> m_path;
    Time m_last_modified_time { Time::now_monotonic() };

    RefPtr<GUI::Model> m_alignment_model;

    Array<RefPtr<GUI::ComboBox>, to_underlying(Gfx::AlignmentRole::__Count)> m_alignment_inputs;
    Array<RefPtr<GUI::ColorInput>, to_underlying(Gfx::ColorRole::__Count)> m_color_inputs;
    Array<RefPtr<GUI::CheckBox>, to_underlying(Gfx::FlagRole::__Count)> m_flag_inputs;
    Array<RefPtr<GUI::SpinBox>, to_underlying(Gfx::MetricRole::__Count)> m_metric_inputs;
    Array<RefPtr<GUI::TextBox>, to_underlying(Gfx::PathRole::__Count)> m_path_inputs;
};

}
