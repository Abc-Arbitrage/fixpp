/* v42.h
   Mathieu Stefani, 12 november 2016
   
   Messages for FIX 4.2
*/

#pragma once

#include <fixpp/versions/base.h>
#include <fixpp/tag.h>
#include <fixpp/dsl.h>

namespace Fix
{
    FIX_BEGIN_VERSION_NAMESPACE(v42, "FIX.4.2")
    {
        namespace Message
        {
            using Heartbeat = MessageV<'0', Tag::TestReqID>;

            using TestRequest = MessageV<'1', Required<Tag::TestReqID>>;

            using ResendRequest = MessageV<'2', Required<Tag::BeginSeqNo>, Required<Tag::EndSeqNo>>;

            using Reject = MessageV<'3',
                            Required<Tag::RefSeqNum>, Tag::RefTagID, Tag::RefMsgType, Tag::SessionRejectReason,
                            Tag::Text, Tag::EncodedTextLen, Tag::EncodedText>;

            using SequenceReset = MessageV<'4', Tag::GapFillFlag, Required<Tag::NewSeqNo>>;

            using Logout = MessageV<'5', Tag::Text, Tag::EncodedTextLen, Tag::EncodedText>;

            using IndicationOfInterest = MessageV<'6',
                  Required<Tag::IOIid>, Required<Tag::IOITransType>, Tag::IOIRefID, Required<Tag::Symbol>, Tag::SymbolSfx,
                  Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear,
                  Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute,
                  Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange,
                  Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                  Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Required<Tag::Side>,
                  Required<Tag::IOIShares>, Tag::Price, Tag::Currency, Tag::ValidUntilTime,
                  Tag::IOIQltyInd, Tag::IOINaturalFlag, RepeatingGroup<Tag::NoIOIQualifiers, Tag::IOIQualifier>,
                  Tag::Text, Tag::EncodedTextLen, Tag::EncodedText, Tag::TransactTime, Tag::URLLink,
                  RepeatingGroup<Tag::NoRoutingIDs, Tag::RoutingType, Tag::RoutingID>,
                  Tag::SpreadToBenchmark, Tag::Benchmark>;

            using Logon = MessageV<
                             'A',
                              Required<Tag::EncryptMethod>, Required<Tag::HeartBtInt>, Tag::RawDataLength, Tag::RawData,
                              Tag::ResetSeqNumFlag, Tag::MaxMessageSize,
                              RepeatingGroup<Tag::NoMsgTypes, Tag::RefMsgType, Tag::MsgDirection>
                           >;

            using MarketDataRequest = MessageV<'V',
                  Required<Tag::MDReqID>, Required<Tag::SubscriptionRequestType>,
                  Required<Tag::MarketDepth>, Tag::MDUpdateType, Tag::AggregatedBook,
                  RepeatingGroup<Tag::NoMDEntryTypes, Tag::MDEntryType>,
                  RepeatingGroup<Tag::NoRelatedSym,
                     Tag::Symbol, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource,
                     Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay,
                     Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier,
                     Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen,
                     Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen,
                     Tag::EncodedSecurityDesc, Tag::TradingSessionID
                 >
             >;

            using MarketDataSnapshot = MessageV<'W',
                  Tag::MDReqID, Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID,
                  Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay,
                  Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier,
                  Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen,
                  Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen,
                  Tag::EncodedSecurityDesc, Tag::FinancialStatus, Tag::CorporateAction, Tag::TotalVolumeTraded,
                  RepeatingGroup<Tag::NoMDEntries,
                       Tag::MDEntryType, Tag::MDEntryPx, Tag::Currency, Tag::MDEntrySize,
                       Tag::MDEntryDate, Tag::MDEntryTime, Tag::TickDirection, Tag::MDMkt,
                       Tag::TradingSessionID, Tag::QuoteCondition, Tag::TradeCondition,
                       Tag::MDEntryOriginator, Tag::LocationID, Tag::DeskID, Tag::OpenCloseSettleFlag,
                       Tag::TimeInForce, Tag::ExpireDate, Tag::ExpireTime, Tag::MinQty,
                       Tag::ExecInst, Tag::SellerDays, Tag::OrderID, Tag::QuoteEntryID,
                       Tag::MDEntryBuyer, Tag::MDEntrySeller, Tag::NumberOfOrders, Tag::MDEntryPositionNo,
                       Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
                   >
               >;

            using MarketDataIncrementalRefresh = MessageV<'X',
                  Tag::MDReqID,
                  Required<RepeatingGroup<Tag::NoMDEntries,
                      Required<Tag::MDUpdateAction>, Tag::DeleteReason, Tag::MDEntryType, Tag::MDEntryID,
                      Tag::MDEntryRefID, Tag::Symbol, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource,
                      Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall,
                      Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate,
                      Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer,
                      Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc,
                      Tag::FinancialStatus, Tag::CorporateAction, Tag::MDEntryPx, Tag::Currency,
                      Tag::MDEntrySize, Tag::MDEntryDate, Tag::MDEntryTime, Tag::TickDirection,
                      Tag::MDMkt, Tag::TradingSessionID, Tag::QuoteCondition, Tag::TradeCondition,
                      Tag::MDEntryOriginator, Tag::LocationID, Tag::DeskID, Tag::OpenCloseSettleFlag,
                      Tag::TimeInForce, Tag::ExpireDate, Tag::ExpireTime, Tag::MinQty, Tag::ExecInst,
                      Tag::SellerDays, Tag::OrderID, Tag::QuoteEntryID, Tag::MDEntryBuyer, Tag::MDEntrySeller,
                      Tag::NumberOfOrders, Tag::MDEntryPositionNo, Tag::TotalVolumeTraded, Tag::Text,
                      Tag::EncodedTextLen, Tag::EncodedText
                >>
            >;

        } // namespace Message

        using Header = MessageBase<
                          Field,
                          Tag::MsgType,
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
