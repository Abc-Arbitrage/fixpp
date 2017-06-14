/* application.h
   Mathieu Stefani, 13 june 2017
   
   Application messages definition for FIX 4.3
*/

#pragma once

#include <fixpp/message_type.h>
#include <fixpp/versions/base.h>
#include <fixpp/versions/v43/component_blocks.h>
#include <fixpp/versions/v43/version.h>
#include <fixpp/dsl.h>

namespace Fixpp
{
	FIXPP_BEGIN_VERSION_NAMESPACE(v43, "FIX.4.3")
	{
		namespace Message
		{
			
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
		} // namespace Message
		
		namespace Spec
		{
			
			struct Application
            {
                using IndicationOfIntereset = Message::IndicationOfInterest;
            };
			
		} // namespace Spec
	}
	FIXPP_END_VERSION_NAMESPACE
} // namespace Fixpp