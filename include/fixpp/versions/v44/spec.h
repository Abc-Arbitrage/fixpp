/* v44/spec.h
   Mathieu Stefani, 15 april 2017
   
  Specification definiation for FIX.4.4
*/

#pragma once

namespace Fixpp
{
    namespace v44
    {
        namespace Spec
        {
            struct Session
            {
                using Header = Fixpp::v44::Header;

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
                using MarketDataSnapshot = Message::MarketDataSnapshot;
                using UserRequest = Message::UserRequest;
                using UserResponse = Message::UserResponse;
            };

            struct Dictionary
            {
                using Version = Fixpp::v44::Version;
                using Header = Fixpp::v44::Header;
                
                using Messages = typename meta::typelist::make<
                    Message::Heartbeat, Message::TestRequest, Message::ResendRequest,
                    Message::Reject, Message::SequenceReset, Message::Logout, Message::Logon,
                    Message::MarketDataSnapshot, Message::UserRequest, Message::UserResponse
                >::Result;
            };

        } // namespace Spec
    } // namespace v44
} // namespace Fixpp
