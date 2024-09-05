/* dict.h
   Mathieu Stefani, 15 april 2017
   
  Specification definiation for FIX.4.4
*/

#pragma once

#include <fixpp/versions/base.h>
#include <fixpp/versions/v50sp2/messages.h>

namespace Fixpp
{
    FIXPP_BEGIN_VERSION_NAMESPACE(v50sp2, "FIXT.1.1")
    {
        namespace Spec
        {
            struct Dictionary
            {
                using Version = Fixpp::v50sp2::Version;
                using Header = Fixpp::v50sp2::Header;
                
                using Messages = typename meta::typelist::make<
                    Message::Heartbeat,
                    Message::TestRequest,
                    Message::MarketDataSnapshot,
                    Message::UserRequest,
                    Message::UserResponse
                >::Result;
            };

        } // namespace Spec
    }
    FIXPP_END_VERSION_NAMESPACE
} // namespace Fixpp
