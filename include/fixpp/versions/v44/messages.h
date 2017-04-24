/* v44.h
   Mathieu Stefani, 04 january 2017
   
   Messages definition for FIX 4.4
*/

#pragma once

#include <fixpp/versions/base.h>
#include <fixpp/versions/v44/component_blocks.h>
#include <fixpp/dsl.h>

namespace Fix
{
    FIX_BEGIN_VERSION_NAMESPACE(v44, "FIX.4.4")
    {

        namespace Message
        {
            using Heartbeat = MessageV<Chars<'0'>, Tag::TestReqID>;

            using TestRequest = MessageV<Chars<'1'>, Required<Tag::TestReqID>>;

            using MarketDataSnapshot = MessageV<Chars<'W'>,
                  Tag::MDReqID,
                  Component::Instrument,
                  RepeatingGroup<
                      Tag::NoUnderlyings,
                      Component::UnderlyingInstrument
                  >,
                  RepeatingGroup<
                      Tag::NoLegs,
                      Component::InstrumentLeg
                  >,
                  Tag::FinancialStatus, Tag::CorporateAction, Tag::NetChgPrevDay,
                  Required<RepeatingGroup<
                      Tag::NoMDEntries,
                      Required<Tag::MDEntryType>, Tag::MDEntryPx, Tag::Currency, Tag::MDEntrySize,
                      Tag::MDEntryDate, Tag::MDEntryTime, Tag::TickDirection, Tag::MDMkt,
                      Tag::TradingSessionID, Tag::TradingSessionSubID, Tag::QuoteCondition,
                      Tag::TradeCondition, Tag::MDEntryOriginator, Tag::LocationID, Tag::DeskID,
                      Tag::OpenCloseSettleFlag, Tag::TimeInForce, Tag::ExpireDate, Tag::ExpireTime,
                      Tag::MinQty, Tag::ExecInst, Tag::SellerDays, Tag::OrderID, Tag::QuoteEntryID,
                      Tag::MDEntryBuyer, Tag::MDEntrySeller, Tag::NumberOfOrders, Tag::Scope, Tag::PriceDelta,
                      Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
                  >>,
                  Tag::ApplQueueDepth, Tag::ApplQueueResolution
              >;

        } // namespace Message

        using Header = StandardMessage<
                          Tag::MsgSeqNum,
                          Required<Tag::SenderCompID>, Required<Tag::TargetCompID>, Tag::OnBehalfOfCompID, Tag::DeliverToCompID,
                          Tag::SecureDataLen, Tag::SecureData, Required<Tag::MsgSeqNum>, Tag::SenderSubID,
                          Tag::SenderLocationID, Tag::TargetSubID, Tag::TargetLocationID, Tag::OnBehalfOfSubID,
                          Tag::OnBehalfOfLocationID, Tag::DeliverToSubID, Tag::DeliverToLocationID,
                          Tag::PossDupFlag, Tag::PossResend, Required<Tag::SendingTime>, Tag::OrigSendingTime,
                          Tag::XmlDataLen, Tag::XmlData, Tag::MessageEncoding,
                          Tag::LastMsgSeqNumProcessed, Tag::OnBehalfOfSendingTime>;

    }
    FIX_END_VERSION_NAMESPACE

} // namespace Fix
