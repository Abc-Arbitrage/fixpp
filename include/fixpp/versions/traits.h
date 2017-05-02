/* visitor.h
   Mathieu Stefani, 25 april 2017
   
   A bunch of type traits for a given FIX version
*/

#pragma once

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v43.h>
#include <fixpp/versions/v44.h>

namespace Fixpp
{
    template<typename Version>
    struct VersionTraits;

    template<>
    struct VersionTraits<Fixpp::v42::Version>
    {
        using Session = Fixpp::v42::Spec::Session;
        using Application = Fixpp::v42::Spec::Application;

        using Dictionary = Fixpp::v42::Spec::Dictionary;
        using Header = Fixpp::v42::Header;
    };

    template<>
    struct VersionTraits<Fixpp::v43::Version>
    {
        using Session = Fixpp::v43::Spec::Session;
        using Application = Fixpp::v43::Spec::Application;

        using Dictionary = Fixpp::v43::Spec::Dictionary;
        using Header = Fixpp::v43::Header;
    };

    template<>
    struct VersionTraits<Fixpp::v44::Version>
    {
        using Session = Fixpp::v44::Spec::Session;
        using Application = Fixpp::v44::Spec::Application;

        using Dictionary = Fixpp::v44::Spec::Dictionary;
        using Header = Fixpp::v44::Header;
    };
} // namespace Fix
