@GUI::Widget {
    layout: @GUI::HorizontalBoxLayout {}
    fill_with_background_color: true

    @GUI::TabWidget {
        name: "preview_tabs"

        @GUI::Frame {
            layout: @GUI::HorizontalBoxLayout {}
            title: "Windows"
            name: "preview_frame"
        }

        @GUI::Widget {
            layout: @GUI::VerticalBoxLayout {}
            title: "Taskbar"
        }

        @GUI::Widget {
            layout: @GUI::VerticalBoxLayout {}
            title: "Widgets"
        }

        @GUI::Widget {
            layout: @GUI::VerticalBoxLayout {}
            title: "Text Editor"
        }
    }

    @GUI::TabWidget {
        name: "property_tabs"

        @GUI::Widget {
            layout: @GUI::VerticalBoxLayout {}
            title: "Window"

            @GUI::Widget {
                layout: @GUI::HorizontalBoxLayout {
                    margins: [4, 4, 4, 4]
                }
                shrink_to_fit: true

                @GUI::Label {
                    text: "ActiveWindowTitle"
                    fixed_width: 230
                    text_alignment: "CenterLeft"
                }

                @GUI::ColorInput {
                    name: "color_input"
                }
            }

            @GUI::Widget {
                layout: @GUI::HorizontalBoxLayout {
                    margins: [4, 4, 4, 4]
                }
                shrink_to_fit: true

                @GUI::Label {
                    text: "ActiveWindowTitleShadow"
                    fixed_width: 230
                    text_alignment: "CenterLeft"
                }

                @GUI::ColorInput {
                    name: "color_input"
                }
            }

            @GUI::Widget {
                layout: @GUI::HorizontalBoxLayout {
                    margins: [4, 4, 4, 4]
                }
                shrink_to_fit: true

                @GUI::Label {
                    text: "ActiveWindowTitleStripes"
                    fixed_width: 230
                    text_alignment: "CenterLeft"
                }

                @GUI::ColorInput {
                    name: "color_input"
                }
            }
        }

        @GUI::Widget {
            layout: @GUI::VerticalBoxLayout {}
            title: "Widgets"

            @GUI::GroupBox {
                layout: @GUI::HorizontalBoxLayout {
                    margins: [4, 4, 4, 4]
                }
                shrink_to_fit: true
                title: "Colors"

                @GUI::ComboBox {
                    name: "color_combo_box"
                    model_only: true
                    fixed_width: 230
                }

                @GUI::ColorInput {
                    name: "color_input"
                }
            }

            @GUI::GroupBox {
                layout: @GUI::HorizontalBoxLayout {
                    margins: [4, 4, 4, 4]
                }
                shrink_to_fit: true
                title: "Alignments"

                @GUI::ComboBox {
                    name: "alignment_combo_box"
                    model_only: true
                    fixed_width: 230
                }

                @GUI::ComboBox {
                    name: "alignment_input"
                }
            }

            @GUI::GroupBox {
                layout: @GUI::HorizontalBoxLayout {
                    margins: [4, 4, 4, 4]
                }
                shrink_to_fit: true
                title: "Flags"

                @GUI::ComboBox {
                    name: "flag_combo_box"
                    model_only: true
                    fixed_width: 230
                }

                @GUI::Widget {}

                @GUI::CheckBox {
                    name: "flag_input"
                    fixed_width: 13
                }
            }

            @GUI::GroupBox {
                layout: @GUI::HorizontalBoxLayout {
                    margins: [4, 4, 4, 4]
                }
                shrink_to_fit: true
                title: "Metrics"

                @GUI::ComboBox {
                    name: "metric_combo_box"
                    model_only: true
                    fixed_width: 230
                }

                @GUI::SpinBox {
                    name: "metric_input"
                }
            }

            @GUI::GroupBox {
                layout: @GUI::HorizontalBoxLayout {
                    margins: [4, 4, 4, 4]
                }
                shrink_to_fit: true
                title: "Paths"

                @GUI::ComboBox {
                    name: "path_combo_box"
                    model_only: true
                    fixed_width: 230
                }

                @GUI::TextBox {
                    name: "path_input"
                    mode: "Editable"
                }

                @GUI::Button {
                    name: "path_picker_button"
                    fixed_width: 20
                    text: "..."
                    tooltip: "Choose..."
                }
            }
        }

        @GUI::Widget {
            layout: @GUI::VerticalBoxLayout {}
            title: "Syntax Highlighting"
        }
    }
}
