/* dict.h
   Mathieu Stefani, 15 april 2017
   
  Specification definiation for FIX.4.3
*/

#pragma once

#include <fixpp/versions/base.h>
#include <fixpp/versions/v43/messages.h>

namespace Fixpp
{
    FIXPP_BEGIN_VERSION_NAMESPACE(v43, "FIX.4.3")
    {
        namespace Spec
        {

            struct Dictionary
            {
                using Version = Fixpp::v43::Version;
                using Header = Fixpp::v43::Header;
                
                using Messages = typename meta::typelist::make<
                    Message::Heartbeat, Message::TestRequest, Message::ResendRequest, Message::Reject, Message::SequenceReset,
                    Message::Logout, Message::Logon
                >::Result;
            };

        } // namespace Spec
    }
	FIXPP_END_VERSION_NAMESPACE
} // namespace Fixpp
