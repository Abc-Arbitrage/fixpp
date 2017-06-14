/* dict.h
   Mathieu Stefani, 15 april 2017
   
  Specification definiation for FIX.4.2
*/

#pragma once

#include <fixpp/versions/v42/messages.h>

namespace Fixpp
{
    FIXPP_BEGIN_VERSION_NAMESPACE(v42, "FIX.4.2")
    {
        namespace Spec
        {
            struct Dictionary
            {
                using Version = Fixpp::v42::Version;
                using Header = Fixpp::v42::Header;

                using Messages = typename meta::typelist::make<
                    Message::Heartbeat, Message::TestRequest, Message::ResendRequest, Message::Reject, Message::SequenceReset,
                    Message::Logout, Message::Logon, Message::MarketDataIncrementalRefresh, Message::Quote,
                    Message::IndicationOfInterest, Message::Advertisement, Message::ExecutionReport,
                    Message::OrderCancelReject, Message::News, Message::Email, Message::NewOrderSingle, Message::NewOrderList, Message::OrderCancelRequest,
                    Message::OrderStatusRequest, Message::Allocation,
                    Message::ListCancelRequest, Message::ListExecute, Message::ListStatusRequest, Message::ListStatusRequest,
                    Message::ListStatus, Message::AllocationInstructionAck, Message::DontKnowTrade, Message::QuoteRequest, Message::Quote,
                    Message::SettlementInstructions, Message::MarketDataRequest, Message::MarketDataSnapshot, Message::MarketDataIncrementalRefresh,
                    Message::MarketDataRequestReject, Message::QuoteCancel, Message::QuoteStatusRequest, Message::MassQuote,
                    Message::BusinessMessageReject, Message::BidRequest, Message::BidResponse, Message::ListStrikePrice
                >::Result;
            };

        }
    }
	FIXPP_END_VERSION_NAMESPACE
} // namespace Fixpp
