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
            using Heartbeat = MessageT<'0', Tag::TestReqID>;
            using Logon = MessageT<
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
