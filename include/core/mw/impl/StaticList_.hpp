/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/namespace.hpp>
#include <core/common.hpp>

NAMESPACE_CORE_MW_BEGIN


class StaticList_:
    private core::Uncopyable
{
public:
    struct Link {
        Link* nextp;
        void* itemp;

        Link(
            void* itemp
        ) :
            nextp(NULL),
            itemp(itemp)
        {}
    };

    typedef bool (* Predicate)(
        const void* itemp
    );
    typedef bool (* Matches)(
        const void* itemp,
        const void* featuresp
    );

private:
    Link* headp;

public:
    const Link*
    get_head_unsafe() const;

    bool
    is_empty_unsafe() const;

    size_t
    count_unsafe() const;

    void
    link_unsafe(
        Link& link
    );

    bool
    unlink_unsafe(
        Link& link
    );

    int
    index_of_unsafe(
        const void* itemp
    ) const;

    bool
    contains_unsafe(
        const void* itemp
    ) const;

    void*
    find_first_unsafe(
        Predicate pred_func
    ) const;

    void*
    find_first_unsafe(
        Matches     match_func,
        const void* featuresp
    ) const;

    const Link*
    get_head() const;

    bool
    is_empty() const;

    size_t
    count() const;

    void
    link(
        Link& link
    );

    bool
    unlink(
        Link& link
    );

    int
    index_of(
        const void* itemp
    ) const;

    bool
    contains(
        const void* itemp
    ) const;

    void*
    find_first(
        Predicate pred_func
    ) const;

    void*
    find_first(
        Matches     match_func,
        const void* featuresp
    ) const;


public:
    StaticList_();
};


inline
const StaticList_::Link*
StaticList_::get_head_unsafe() const
{
    return headp;
}

inline
bool
StaticList_::is_empty_unsafe() const
{
    return headp == NULL;
}

inline
void
StaticList_::link_unsafe(
    Link& link
)
{
    CORE_ASSERT(link.nextp == NULL);
    CORE_ASSERT(!contains_unsafe(link.itemp));

    link.nextp = headp;
    headp      = &link;
}

NAMESPACE_CORE_MW_END
