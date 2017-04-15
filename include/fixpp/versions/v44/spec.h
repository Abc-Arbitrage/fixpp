/* v44/spec.h
   Mathieu Stefani, 15 april 2017
   
  Specification definiation for FIX.4.4
*/

#pragma once

namespace Fix
{
    namespace v44
    {
        namespace Spec
        {
            struct Session
            {
                using Header = Fix::v44::Header;

                using Heartbeat = Message::Heartbeat;
                using TestRequest = Message::TestRequest;
            };

            struct Application
            {
                using MarketDataSnapshot = Message::MarketDataSnapshot;
            };

            struct Dictionary
            {
                using Version = Fix::v44::Version;
                using Header = Fix::v44::Header;
                
                using Messages = typename meta::typelist::make<
                    Message::Heartbeat, Message::TestRequest,
                    Message::MarketDataSnapshot
                >::Result;
            };

        } // namespace Spec
    } // namespace v44
} // namespace Fix
