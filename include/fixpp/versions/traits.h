/* visitor.h
   Mathieu Stefani, 25 april 2017
   
   A bunch of type traits for a given FIX version
*/

#pragma once

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v43.h>
#include <fixpp/versions/v44.h>

namespace Fix
{
    template<typename Version>
    struct VersionTraits;

    template<>
    struct VersionTraits<Fix::v42::Version>
    {
        using Session = Fix::v42::Spec::Session;
        using Application = Fix::v42::Spec::Application;

        using Dictionary = Fix::v42::Spec::Dictionary;
        using Header = Fix::v42::Header;
    };

    template<>
    struct VersionTraits<Fix::v43::Version>
    {
        using Session = Fix::v43::Spec::Session;
        using Application = Fix::v43::Spec::Application;

        using Dictionary = Fix::v43::Spec::Dictionary;
        using Header = Fix::v43::Header;
    };

    template<>
    struct VersionTraits<Fix::v44::Version>
    {
        using Session = Fix::v44::Spec::Session;
        using Application = Fix::v44::Spec::Application;

        using Dictionary = Fix::v44::Spec::Dictionary;
        using Header = Fix::v44::Header;
    };
} // namespace Fix
