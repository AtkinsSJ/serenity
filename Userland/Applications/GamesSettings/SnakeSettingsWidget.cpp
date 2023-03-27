/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "SnakeSettingsWidget.h"
#include <Applications/GamesSettings/SnakeSettingsWidgetGML.h>
#include <LibConfig/Client.h>
#include <LibCore/Directory.h>
#include <LibGUI/Frame.h>
#include <LibGUI/ItemListModel.h>
#include <LibGUI/Painter.h>

// I apologise for this abomination but I don't want to duplicate all that code.
#include <Games/Snake/Skins/ClassicSkin.cpp>
#include <Games/Snake/Skins/ImageSkin.cpp>
#include <Games/Snake/Skins/SnakeSkin.cpp>

namespace GamesSettings {

static constexpr StringView default_snake_skin_name = "classic"sv;
static constexpr Color default_snake_color = Color::Green;

class SnakeGamePreview final : public GUI::Frame {
    C_OBJECT_ABSTRACT(ChessGamePreview)
public:
    static ErrorOr<NonnullRefPtr<SnakeGamePreview>> try_create()
    {
        return adopt_nonnull_ref_or_enomem(new (nothrow) SnakeGamePreview());
    }

    virtual ~SnakeGamePreview() = default;

    ErrorOr<void> set_snake_skin_name(StringView name)
    {
        if (m_snake_skin_name == name)
            return {};

        m_snake_skin_name = TRY(String::from_utf8(name));

        m_skin = TRY(Snake::SnakeSkin::create(m_snake_skin_name, m_snake_color));
        update();
        return {};
    }

    ErrorOr<void> set_snake_color(Color color)
    {
        if (m_snake_color == color)
            return {};

        m_snake_color = color;

        m_skin = TRY(Snake::SnakeSkin::create(m_snake_skin_name, m_snake_color));
        update();
        return {};
    }

private:
    SnakeGamePreview() = default;

    virtual void paint_event(GUI::PaintEvent& event) override
    {
        GUI::Frame::paint_event(event);

        GUI::Painter painter { *this };
        painter.add_clip_rect(event.rect());
        painter.add_clip_rect(frame_inner_rect());

        painter.fill_rect(event.rect(), m_background_color);
    }

    String m_snake_skin_name;
    Color m_snake_color;
    Color m_background_color { Color::Black };
    OwnPtr<Snake::SnakeSkin> m_skin;
};

ErrorOr<NonnullRefPtr<SnakeSettingsWidget>> SnakeSettingsWidget::try_create()
{
    auto snake_settings_widget = TRY(adopt_nonnull_ref_or_enomem(new (nothrow) SnakeSettingsWidget));
    TRY(snake_settings_widget->initialize());
    return snake_settings_widget;
}

ErrorOr<void> SnakeSettingsWidget::initialize()
{
    TRY(load_from_gml(snake_settings_widget_gml));

    auto snake_skin_name = Config::read_string("Games"sv, "Snake"sv, "SnakeSkin"sv, default_snake_skin_name);
    auto snake_color = Color::from_argb(Config::read_u32("Games"sv, "Snake"sv, "BaseColor"sv, default_snake_color.value()));

    m_preview = find_descendant_of_type_named<SnakeGamePreview>("snake_preview");

    m_snake_skin_combobox = find_descendant_of_type_named<GUI::ComboBox>("snake_skin");
    TRY(m_snake_skins.try_append(TRY(String::from_utf8("classic"sv))));
    TRY(Core::Directory::for_each_entry("/res/graphics/snake/skins/"sv, Core::DirIterator::SkipParentAndBaseDir, [&](auto const& entry, auto&) -> ErrorOr<IterationDecision> {
        TRY(m_snake_skins.try_append(TRY(String::from_utf8(entry.name))));
        return IterationDecision::Continue;
    }));
    auto snake_skin_model = TRY(GUI::ItemListModel<String>::try_create(m_snake_skins));
    m_snake_skin_combobox->set_model(snake_skin_model);
    m_snake_skin_combobox->set_text(snake_skin_name, GUI::AllowCallback::No);
    m_snake_skin_combobox->on_change = [&](auto& value, auto&) {
        set_modified(true);
        m_snake_color_label->set_enabled(value == "classic"sv);
        m_snake_color_input->set_enabled(value == "classic"sv);

        m_preview->set_snake_skin_name(value).release_value_but_fixme_should_propagate_errors();
    };

    m_snake_color_label = find_descendant_of_type_named<GUI::Label>("snake_color_label");
    m_snake_color_label->set_enabled(snake_skin_name == "classic"sv);

    m_snake_color_input = find_descendant_of_type_named<GUI::ColorInput>("snake_color");
    m_snake_color_input->set_enabled(snake_skin_name == "classic"sv);
    m_snake_color_input->set_color(snake_color, GUI::AllowCallback::No);
    m_snake_color_input->on_change = [&] {
        set_modified(true);
        m_snake_color_input->set_color(m_snake_color_input->color().with_alpha(255));
        m_preview->set_snake_color(m_snake_color_input->color()).release_value_but_fixme_should_propagate_errors();
    };

    TRY(m_preview->set_snake_skin_name(m_snake_skin_combobox->text()));
    TRY(m_preview->set_snake_color(m_snake_color_input->color()));

    return {};
}

void SnakeSettingsWidget::apply_settings()
{
    Config::write_string("Games"sv, "Snake"sv, "SnakeSkin"sv, m_snake_skin_combobox->text());
    Config::write_u32("Games"sv, "Snake"sv, "BaseColor"sv, m_snake_color_input->color().value());
}

void SnakeSettingsWidget::reset_default_values()
{
    // FIXME: `set_text()` on a combobox doesn't trigger the `on_change` callback, but it probably should!
    //        Until then, we have to manually tell the preview to update.
    m_snake_skin_combobox->set_text(default_snake_skin_name);
    m_snake_color_label->set_enabled(default_snake_skin_name == "classic"sv);
    m_snake_color_input->set_enabled(default_snake_skin_name == "classic"sv);
    m_snake_color_input->set_color(default_snake_color);

    m_preview->set_snake_skin_name(m_snake_skin_combobox->text()).release_value_but_fixme_should_propagate_errors();
    m_preview->set_snake_color(m_snake_color_input->color()).release_value_but_fixme_should_propagate_errors();
}

}

REGISTER_WIDGET(GamesSettings, SnakeGamePreview);
