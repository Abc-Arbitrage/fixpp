/* messages.h
   Mathieu Stefani, 13 april 2017
   
   Messages definition for FIX 4.3
*/

#pragma once

#include <fixpp/versions/base.h>
#include <fixpp/versions/v43/component_blocks.h>
#include <fixpp/dsl.h>

namespace Fix
{
    FIX_BEGIN_VERSION_NAMESPACE(v43, "FIX.4.3")
    {

        namespace Message
        {
            using Heartbeat = MessageV<Chars<'0'>, Tag::TestReqID>;

            using TestRequest = MessageV<Chars<'1'>, Required<Tag::TestReqID>>;

            using ResendRequest = MessageV<Chars<'2'>, Required<Tag::BeginSeqNo>, Required<Tag::EndSeqNo>>;

            using Reject = MessageV<Chars<'3'>,
                            Required<Tag::RefSeqNum>, Tag::RefTagID, Tag::RefMsgType, Tag::SessionRejectReason,
                            Tag::Text, Tag::EncodedTextLen, Tag::EncodedText>;

            using SequenceReset = MessageV<Chars<'4'>, Tag::GapFillFlag, Required<Tag::NewSeqNo>>;

            using Logout = MessageV<Chars<'5'>, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText>;

            using IndicationOfInterest = MessageV<Chars<'6'>,
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

            using Logon = MessageV<Chars<'A'>,
                  Required<Tag::EncryptMethod>, Required<Tag::HeartBtInt>, Tag::RawDataLength, Tag::RawData,
                  Tag::ResetSeqNumFlag, Tag::MaxMessageSize,
                  RepeatingGroup<
                      Tag::NoMsgTypes,
                      Tag::RefMsgType, Tag::MsgDirection
                  >,
                  Tag::TestMessageIndicator, Tag::Username, Tag::Password
             >;

        } // namespace Mesage

        using Header = StandardMessage<
                          Tag::MsgSeqNum,
                          Required<Tag::SenderCompID>, Required<Tag::TargetCompID>, Tag::OnBehalfOfCompID, Tag::DeliverToCompID,
                          Tag::SecureDataLen, Tag::SecureData, Required<Tag::MsgSeqNum>, Tag::SenderSubID,
                          Tag::SenderLocationID, Tag::TargetSubID, Tag::TargetLocationID, Tag::OnBehalfOfSubID,
                          Tag::OnBehalfOfLocationID, Tag::DeliverToSubID, Tag::DeliverToLocationID,
                          Tag::PossDupFlag, Tag::PossResend, Required<Tag::SendingTime>, Tag::OrigSendingTime,
                          Tag::XmlDataLen, Tag::XmlData, Tag::MessageEncoding,
                          Tag::LastMsgSeqNumProcessed, Tag::OnBehalfOfSendingTime,
                          RepeatingGroup<
                              Tag::NoHops,
                              Tag::HopCompID, Tag::HopSendingTime, Tag::HopRefID
                          >
                      >;

    }
    FIX_END_VERSION_NAMESPACE

} // namespace Fix

