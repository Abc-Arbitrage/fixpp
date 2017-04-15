/* v43/spec.h
   Mathieu Stefani, 15 april 2017
   
  Specification definiation for FIX.4.3
*/

#pragma once

namespace Fix
{
    namespace v43
    {
        namespace Spec
        {
            struct Session
            {
                using Header = Fix::v43::Header;

                using Heartbeat = Message::Heartbeat;
                using TestRequest = Message::TestRequest;
                using ResendRequest = Message::ResendRequest;
                using Reject = Message::Reject;
                using SequenceReset = Message::SequenceReset;
                using Logout = Message::Logout;
                using Logon = Message::Logon;
            };

            struct Application
            {
                using IndicationOfIntereset = Message::IndicationOfInterest;
            };

            struct Dictionary
            {
                using Version = Fix::v43::Version;
                using Header = Fix::v43::Header;
                
                using Messages = typename meta::typelist::make<
                    Message::Heartbeat, Message::TestRequest, Message::ResendRequest, Message::Reject, Message::SequenceReset,
                    Message::Logout, Message::Logon
                >::Result;
            };

        } // namespace Spec
    } // namespace v43
} // namespace Fix
