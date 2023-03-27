/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Vector.h>
#include <LibGUI/ColorInput.h>
#include <LibGUI/ComboBox.h>
#include <LibGUI/Label.h>
#include <LibGUI/SettingsWindow.h>

namespace GamesSettings {

class SnakeGamePreview;

class SnakeSettingsWidget final : public GUI::SettingsWindow::Tab {
    C_OBJECT_ABSTRACT(ChessSettingsWidget)
public:
    static ErrorOr<NonnullRefPtr<SnakeSettingsWidget>> try_create();
    virtual ~SnakeSettingsWidget() override = default;

    virtual void apply_settings() override;
    virtual void reset_default_values() override;

private:
    SnakeSettingsWidget() = default;
    ErrorOr<void> initialize();

    Vector<String> m_snake_skins;

    RefPtr<SnakeGamePreview> m_preview;
    RefPtr<GUI::ComboBox> m_snake_skin_combobox;
    RefPtr<GUI::Label> m_snake_color_label;
    RefPtr<GUI::ColorInput> m_snake_color_input;
};

}
