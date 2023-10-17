/*
 * Copyright (c) 2023, Sam Atkins <atkinssj@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "CountersSet.h"

namespace Web::CSS {

void CountersSet::append(FlyString name, i32 originating_element_id, bool reversed, Optional<CounterValue> value)
{
    m_counters.append({ .name = move(name),
        .originating_element_id = originating_element_id,
        .reversed = reversed,
        .value = value });
}

Optional<Counter const&> CountersSet::last_counter_with_name(FlyString name) const
{
    for (auto& counter : m_counters.in_reverse()) {
        if (counter.name == name)
            return counter;
    }
    return {};
}

Optional<Counter&> CountersSet::counter_with_same_name_and_creator(FlyString name, i32 originating_element_id)
{
    return m_counters.first_matching([&](auto& it) {
        return it.name == name && it.originating_element_id == originating_element_id;
    });
}

void CountersSet::remove(Counter const& counter)
{
    m_counters.remove_first_matching([&counter](auto& it) { return it == counter; });
}

}
