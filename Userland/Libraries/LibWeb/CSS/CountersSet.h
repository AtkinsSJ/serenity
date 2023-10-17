/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Checked.h>
#include <AK/FlyString.h>
#include <AK/Optional.h>

namespace Web::CSS {

// "UAs may have implementation-specific limits on the maximum or minimum value of a counter.
// If a counter reset, set, or increment would push the value outside of that range, the value
// must be clamped to that range." - https://drafts.csswg.org/css-lists-3/#auto-numbering
// So, we use a Checked<i32> and saturating addition/subtraction.
using CounterValue = Checked<i32>;

// https://drafts.csswg.org/css-lists-3/#counter
struct Counter {
    FlyString name;
    i32 originating_element_id; // "creator"
    bool reversed { false };
    Optional<CounterValue> value;

    bool operator==(Counter const& other) const = default;
};

// https://drafts.csswg.org/css-lists-3/#css-counters-set
class CountersSet {
public:
    CountersSet() = default;
    ~CountersSet() = default;

    void append(FlyString name, i32 originating_element_id, bool reversed, Optional<CounterValue>);
    Optional<Counter const&> last_counter_with_name(FlyString name) const;
    Optional<Counter&> counter_with_same_name_and_creator(FlyString name, i32 originating_element_id);
    void remove(Counter const& counter);

    Vector<Counter> const& counters() const { return m_counters; }
    bool is_empty() const { return m_counters.is_empty(); }

private:
    Vector<Counter> m_counters;
};

}
