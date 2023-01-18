/*
 * Copyright (c) 2019-2020, Sergey Bugaev <bugaevc@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullRefPtr.h>
#include <AK/Optional.h>
#include <AK/Result.h>
#include <AK/String.h>
#include <LibGUI/Model.h>

class ManualNode;

class ManualModel final : public GUI::Model {
public:
    static ErrorOr<NonnullRefPtr<ManualModel>> create();
    virtual ~ManualModel() override;

    Optional<GUI::ModelIndex> index_from_path(StringView) const;
    ErrorOr<GUI::ModelIndex> index_from_url(URL) const;

    Optional<String const&> page_name(GUI::ModelIndex const&) const;
    Optional<String const&> page_path(GUI::ModelIndex const&) const;
    Optional<String> page_and_section(GUI::ModelIndex const&) const;
    ErrorOr<StringView> page_view(String const& path) const;

    void update_section_node_on_toggle(GUI::ModelIndex const&, bool const);
    virtual int row_count(GUI::ModelIndex const& = GUI::ModelIndex()) const override;
    virtual int column_count(GUI::ModelIndex const& = GUI::ModelIndex()) const override;
    virtual GUI::Variant data(GUI::ModelIndex const&, GUI::ModelRole) const override;
    virtual TriState data_matches(GUI::ModelIndex const&, const GUI::Variant&) const override;
    virtual GUI::ModelIndex parent_index(GUI::ModelIndex const&) const override;
    virtual GUI::ModelIndex index(int row, int column = 0, GUI::ModelIndex const& parent = GUI::ModelIndex()) const override;

    void dump() const;

private:
    ManualModel(NonnullRefPtr<Gfx::Bitmap> section_open_icon, NonnullRefPtr<Gfx::Bitmap> section_icon, NonnullRefPtr<Gfx::Bitmap> page_icon);

    ErrorOr<void> load_sections();

    GUI::Icon m_section_open_icon;
    GUI::Icon m_section_icon;
    GUI::Icon m_page_icon;

    Array<RefPtr<ManualNode>, 8> m_manual_sections;
    mutable HashMap<String, NonnullRefPtr<Core::MappedFile>> m_mapped_files;
};
