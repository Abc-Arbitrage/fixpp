/* application.h
   Mathieu Stefani, 13 june 2017
   
   Application messages definition for FIX 4.3
*/

#pragma once

#include <fixpp/message_type.h>
#include <fixpp/versions/base.h>
#include <fixpp/versions/v44/component_blocks.h>
#include <fixpp/versions/v44/version.h>
#include <fixpp/dsl.h>

namespace Fixpp
{
	FIXPP_BEGIN_VERSION_NAMESPACE(v44, "FIX.4.4")
	{
		namespace Message
		{

			using MarketDataSnapshot = MessageV<MessageType::MarketDataSnapshotFullRefresh,
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

            using UserRequest = MessageV<MessageType::UserRequest,
                Required<Tag::UserRequestID>,
                Required<Tag::Username>,
                Tag::UserStatus,
                Tag::UserStatusText
            >;
                        
            using UserResponse = MessageV<MessageType::UserResponse,
                Required<Tag::UserRequestID>,
                Required<Tag::UserRequestType>,
                Required<Tag::Username>,
                Tag::Password,
                Tag::NewPassword,
                Tag::RawDataLength,
                Tag::RawData
            >;
		} // namespace Message
		
		namespace Spec
		{
			
            struct Application
            {
                using MarketDataSnapshot = Message::MarketDataSnapshot;
                using UserRequest = Message::UserRequest;
                using UserResponse = Message::UserResponse;
            };
		} // namespace Spec
	}
	FIXPP_END_VERSION_NAMESPACE
} // namespace Fixpp