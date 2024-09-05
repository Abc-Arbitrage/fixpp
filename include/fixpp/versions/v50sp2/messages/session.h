/* session.h
   Mathieu Stefani, 13 june 2017
   
   Session messages definition for FIX 5.0 SP2
*/

#pragma once

#include <fixpp/versions/base.h>
#include <fixpp/versions/v50sp2/component_blocks.h>
#include <fixpp/dsl.h>
#include <fixpp/versions/v50sp2/version.h>

namespace Fixpp
{
    FIXPP_BEGIN_VERSION_NAMESPACE(v50sp2, "FIXT.1.1")
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

            using Logon = MessageV<Chars<'A'>,
                  Required<Tag::EncryptMethod>, Required<Tag::HeartBtInt>, Tag::RawDataLength, Tag::RawData,
                  Tag::ResetSeqNumFlag, Tag::MaxMessageSize,
                  RepeatingGroup<Tag::NoMsgTypes, Tag::RefMsgType, Tag::MsgDirection>,
                  Tag::Username, Tag::Password,
                  Required<Tag::DefaultApplVerID>
             >;

            using Logout = MessageV<Chars<'5'>, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText>;
		}
		

		
		using Header = StandardMessage<
                          Required<Tag::SenderCompID>, Required<Tag::TargetCompID>, Tag::OnBehalfOfCompID, Tag::DeliverToCompID,
                          Tag::SecureDataLen, Tag::SecureData, Required<Tag::MsgSeqNum>, Tag::SenderSubID,
                          Tag::SenderLocationID, Tag::TargetSubID, Tag::TargetLocationID, Tag::OnBehalfOfSubID,
                          Tag::OnBehalfOfLocationID, Tag::DeliverToSubID, Tag::DeliverToLocationID,
                          Tag::PossDupFlag, Tag::PossResend, Required<Tag::SendingTime>, Tag::OrigSendingTime,
                          Tag::XmlDataLen, Tag::XmlData, Tag::MessageEncoding,
                          Tag::LastMsgSeqNumProcessed, Tag::OnBehalfOfSendingTime>;
		
		namespace Spec
		{
			struct Session
            {
                using Header = Fixpp::v50sp2::Header;

                using Heartbeat = Message::Heartbeat;
                using TestRequest = Message::TestRequest;
                using ResendRequest = Message::ResendRequest;
                using Reject = Message::Reject;
                using SequenceReset = Message::SequenceReset;
                using Logout = Message::Logout;
                using Logon = Message::Logon;
            };
		} 
	}
	FIXPP_END_VERSION_NAMESPACE
} // namespace Fixpp
