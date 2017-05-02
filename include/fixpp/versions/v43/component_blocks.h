/* v43.h
   Mathieu Stefani, 13 april 2017
   
  Component Blocks definition for FIX 4.3
*/

#pragma once

#include <fixpp/dsl/component_block.h>
#include <fixpp/tag.h>

namespace Fixpp
{
    namespace v43
    {
        namespace Component
        {
            using CommissionData = ComponentBlock<
                Tag::Commission, Tag::CommType, Tag::CommCurrency, Tag::FundRenewWaiv
            >;

            using Instrument = ComponentBlock<
                Tag::Symbol, Tag::SymbolSfx, Tag::SecurityID, Tag::SecurityIDSource,
                RepeatingGroup<
                    Tag::NoSecurityAltID,
                    Tag::SecurityAltID, Tag::SecurityAltIDSource
                >,
                Tag::Product, Tag::CFICode, Tag::SecurityType, Tag::SecuritySubType,
                Tag::MaturityMonthYear, Tag::MaturityDate, Tag::CouponPaymentDate,
                Tag::IssueDate, Tag::RepoCollateralSecurityType, Tag::RepurchaseTerm,
                Tag::RepurchaseRate, Tag::Factor, Tag::CreditRating, Tag::InstrRegistry,
                Tag::CountryOfIssue, Tag::StateOrProvinceOfIssue, Tag::LocaleOfIssue,
                Tag::RedemptionDate, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier,
                Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen,
                Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen,
                Tag::EncodedSecurityDesc
             >;

            using InstrumentLeg = ComponentBlock<
                Tag::LegSymbol, Tag::LegSymbolSfx, Tag::LegSecurityID, Tag::LegSecurityIDSource,
                RepeatingGroup<
                    Tag::NoLegSecurityAltID,
                    Tag::LegSecurityAltID, Tag::LegSecurityAltIDSource
                >,
                Tag::LegProduct, Tag::LegCFICode, Tag::LegSecurityType, Tag::LegSecuritySubType,
                Tag::LegMaturityMonthYear, Tag::LegMaturityDate, Tag::LegCouponPaymentDate,
                Tag::LegIssueDate, Tag::LegRepoCollateralSecurityType, Tag::LegRepurchaseTerm,
                Tag::LegRepurchaseRate, Tag::LegFactor, Tag::LegCreditRating, Tag::LegInstrRegistry,
                Tag::LegCountryOfIssue, Tag::LegStateOrProvinceOfIssue, Tag::LegLocaleOfIssue,
                Tag::LegRedemptionDate, Tag::LegStrikePrice, Tag::LegOptAttribute,
                Tag::LegContractMultiplier, Tag::LegCouponRate, Tag::LegSecurityExchange,
                Tag::LegIssuer, Tag::EncodedLegIssuerLen, Tag::EncodedLegIssuer, Tag::LegSecurityDesc,
                Tag::EncodedLegSecurityDescLen, Tag::EncodedLegSecurityDesc, Tag::LegRatioQty,
                Tag::LegSide
            >;

            using NestedParties = ComponentBlock<
                RepeatingGroup<Tag::NoNestedPartyIDs,
                    Tag::NestedPartyID, Tag::NestedPartyIDSource, Tag::NestedPartyRole,
                    Tag::NestedPartySubID
                >
            >;

        } // namespace Component

    } // namesapce v43
} // namespace Fixpp
