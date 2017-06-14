/* session.h
   Mathieu Stefani, 13 june 2017
   
   Session messages definition for FIX 4.2
*/

#pragma once

#include <fixpp/versions/base.h>
#include <fixpp/dsl.h>
#include <fixpp/versions/v42/version.h>

namespace Fixpp
{
	FIXPP_BEGIN_VERSION_NAMESPACE(v42, "FIX.4.2")
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
			
			using Logon = MessageV<Chars<'A'>,
                  Required<Tag::EncryptMethod>, Required<Tag::HeartBtInt>, Tag::RawDataLength, Tag::RawData,
                  Tag::ResetSeqNumFlag, Tag::MaxMessageSize,
                  RepeatingGroup<Tag::NoMsgTypes, Tag::RefMsgType, Tag::MsgDirection>
             >;

        } // namespace Message
		
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
				using Header = Fixpp::v42::Header;

				using Heartbeat = Fixpp::v42::Message::Heartbeat;
				using TestRequest = Fixpp::v42::Message::TestRequest;
				using ResendRequest = Fixpp::v42::Message::ResendRequest;
				using Reject = Fixpp::v42::Message::Reject;
				using SequenceReset = Fixpp::v42::Message::SequenceReset;
				using Logout = Fixpp::v42::Message::Logout;
				using Logon = Fixpp::v42::Message::Logon;
			};

		} // namespace Spec

    }
    FIXPP_END_VERSION_NAMESPACE
} // namespace Fixpp
	