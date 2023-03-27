@GUI::Frame {
    fill_with_background_color: true
    layout: @GUI::VerticalBoxLayout {
        margins: [8]
    }

    @GamesSettings::SnakeGamePreview {
        name: "snake_preview"
        fixed_height: 160
    }

    @GUI::GroupBox {
        title: "Appearance"
        max_height: "shrink"
        layout: @GUI::VerticalBoxLayout {
            margins: [8]
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 16
            }

            @GUI::Label {
                text: "Snake skin:"
                text_alignment: "CenterLeft"
            }

            @GUI::ComboBox {
                name: "snake_skin"
                model_only: true
            }
        }

        @GUI::Widget {
            layout: @GUI::HorizontalBoxLayout {
                spacing: 16
            }

            @GUI::Label {
                name: "snake_color_label"
                text: "Skin color:"
                text_alignment: "CenterLeft"
            }

            @GUI::ColorInput {
                name: "snake_color"
            }
        }
    }
}
