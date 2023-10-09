/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibWeb/CSS/StyleValue.h>

namespace Web::CSS {

// https://drafts.csswg.org/css-lists-3/#counter-functions
class CounterStyleValue : public StyleValueWithDefaultOperators<CounterStyleValue> {
public:
    enum class CounterFunction {
        Counter,
        Counters,
    };

    static ValueComparingNonnullRefPtr<CounterStyleValue> create_counter(ValueComparingNonnullRefPtr<StyleValue const> counter_name, ValueComparingNonnullRefPtr<StyleValue const> counter_style)
    {
        return adopt_ref(*new (nothrow) CounterStyleValue(CounterFunction::Counter, move(counter_name), move(counter_style), nullptr));
    }
    static ValueComparingNonnullRefPtr<CounterStyleValue> create_counters(ValueComparingNonnullRefPtr<StyleValue const> counter_name, ValueComparingNonnullRefPtr<StyleValue const> join_string, ValueComparingNonnullRefPtr<StyleValue const> counter_style)
    {
        return adopt_ref(*new (nothrow) CounterStyleValue(CounterFunction::Counters, move(counter_name), move(counter_style), move(join_string)));
    }
    virtual ~CounterStyleValue() override;

    CounterFunction function_type() const { return m_properties.function; }
    auto counter_name() const { return m_properties.counter_name; }
    auto counter_style() const { return m_properties.counter_style; }
    auto join_string() const { return m_properties.join_string; }

    virtual String to_string() const override;

    bool properties_equal(CounterStyleValue const& other) const;

private:
    explicit CounterStyleValue(CounterFunction, ValueComparingNonnullRefPtr<StyleValue const> counter_name, ValueComparingNonnullRefPtr<StyleValue const> counter_style, ValueComparingRefPtr<StyleValue const> join_string);

    struct Properties {
        CounterFunction function;
        ValueComparingNonnullRefPtr<StyleValue const> counter_name;
        ValueComparingNonnullRefPtr<StyleValue const> counter_style;
        ValueComparingRefPtr<StyleValue const> join_string;
        bool operator==(Properties const&) const = default;
    } m_properties;
};

}
