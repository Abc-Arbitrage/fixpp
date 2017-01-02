/* v42.h
   Mathieu Stefani, 12 november 2016
   
   Messages for FIX 4.2
*/

#pragma once

#include <fixpp/versions/base.h>
#include <fixpp/tag.h>
#include <fixpp/message.h>

namespace Fix
{
    FIX_BEGIN_VERSION_NAMESPACE(v42, "FIX.4.2")
    {
        namespace Message
        {
            using Heartbeat = MessageV<'0', Tag::TestReqID>;

            using TestRequest = MessageV<'1', Tag::TestReqID>;

            using ResendRequest = MessageV<'2', Tag::BeginSeqNo, Tag::EndSeqNo>;

            using Reject = MessageV<'3',
                            Tag::RefSeqNum, Tag::RefTagID, Tag::RefMsgType, Tag::SessionRejectReason,
                            Tag::Text, Tag::EncodedTextLen, Tag::EncodedText>;

            using SequenceReset = MessageV<'4', Tag::GapFillFlag, Tag::NewSeqNo>;

            using Logout = MessageV<'5', Tag::Text, Tag::EncodedTextLen, Tag::EncodedText>;

            using IndicationOfInterest = MessageV<'6',
                  Tag::IOIid, Tag::IOITransType, Tag::IOIRefID, Tag::Symbol, Tag::SymbolSfx,
                  Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear,
                  Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute,
                  Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange,
                  Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                  Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Tag::Side,
                  Tag::IOIShares, Tag::Price, Tag::Currency, Tag::ValidUntilTime,
                  Tag::IOIQltyInd, Tag::IOINaturalFlag, RepeatingGroup<Tag::NoIOIQualifiers, Tag::IOIQualifier>,
                  Tag::Text, Tag::EncodedTextLen, Tag::EncodedText, Tag::TransactTime, Tag::URLLink,
                  RepeatingGroup<Tag::NoRoutingIDs, Tag::RoutingType, Tag::RoutingID>,
                  Tag::SpreadToBenchmark, Tag::Benchmark>;

            using Logon = MessageV<
                             'A',
                              Tag::EncryptMethod, Tag::HeartBtInt, Tag::RawDataLength, Tag::RawData,
                              Tag::ResetSeqNumFlag, Tag::MaxMessageSize,
                              RepeatingGroup<Tag::NoMsgTypes, Tag::RefMsgType, Tag::MsgDirection>
                           >;

        } // namespace Message

        using Header = MessageBase<
                          Field,
                          Tag::MsgType,
                          Tag::SenderCompID, Tag::TargetCompID, Tag::OnBehalfOfCompID, Tag::DeliverToCompID,
                          Tag::SecureDataLen, Tag::SecureData, Tag::MsgSeqNum, Tag::SenderSubID,
                          Tag::SenderLocationID, Tag::TargetSubID, Tag::TargetLocationID, Tag::OnBehalfOfSubID,
                          Tag::OnBehalfOfLocationID, Tag::DeliverToSubID, Tag::DeliverToLocationID,
                          Tag::PossDupFlag, Tag::PossResend, Tag::SendingTime, Tag::OrigSendingTime,
                          Tag::XmlDataLen, Tag::XmlData, Tag::MessageEncoding,
                          Tag::LastMsgSeqNumProcessed, Tag::OnBehalfOfSendingTime>;

    }
    FIX_END_VERSION_NAMESPACE

} // namespace Fix
