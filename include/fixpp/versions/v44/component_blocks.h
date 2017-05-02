/* v44.h
   Mathieu Stefani, 04 january 2017
   
  Component Blocks definition for FIX 4.4
*/

#pragma once

#include <fixpp/dsl/component_block.h>
#include <fixpp/tag.h>

namespace Fixpp
{
    namespace v44
    {
        namespace Component
        {

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
                Tag::RedemptionDate, Tag::StrikePrice, Tag::StrikeCurrency,
                Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange,
                Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Tag::Pool,
                Tag::ContractSettlMonth, Tag::CPProgram,
                RepeatingGroup<
                   Tag::NoEvents,
                   Tag::EventType, Tag::EventDate, Tag::EventPx, Tag::EventText
                 >,
                 Tag::DatedDate, Tag::InterestAccrualDate
             >;

            using UnderlyingStipulations = ComponentBlock<
                RepeatingGroup<
                    Tag::NoUnderlyingStips,
                    Tag::UnderlyingStipType,
                    Tag::UnderlyingStipValue
                >
            >;

            using UnderlyingInstrument = ComponentBlock<
                Tag::UnderlyingSymbol, Tag::UnderlyingSymbolSfx, Tag::UnderlyingSecurityID,
                RepeatingGroup<
                    Tag::NoUnderlyingSecurityAltID,
                    Tag::UnderlyingSecurityAltID, Tag::UnderlyingSecurityAltIDSource
                >,
                Tag::UnderlyingProduct, Tag::UnderlyingCFICode, Tag::UnderlyingSecurityType,
                Tag::UnderlyingSecuritySubType, Tag::UnderlyingMaturityMonthYear,
                Tag::UnderlyingMaturityDate, Tag::UnderlyingCouponPaymentDate,
                Tag::UnderlyingIssueDate, Tag::UnderlyingRepoCollateralSecurityType,
                Tag::UnderlyingRepurchaseTerm, Tag::UnderlyingRepurchaseRate,
                Tag::UnderlyingFactor, Tag::UnderlyingCreditRating, Tag::UnderlyingInstrRegistry,
                Tag::UnderlyingCountryOfIssue, Tag::UnderlyingStateOrProvinceOfIssue,
                Tag::UnderlyingLocaleOfIssue, Tag::UnderlyingRedemptionDate, Tag::UnderlyingStrikePrice,
                Tag::UnderlyingStrikeCurrency, Tag::UnderlyingOptAttribute,
                Tag::UnderlyingContractMultiplier, Tag::UnderlyingCouponRate,
                Tag::UnderlyingSecurityExchange, Tag::UnderlyingIssuer, Tag::EncodedUnderlyingIssuerLen,
                Tag::EncodedUnderlyingIssuer, Tag::UnderlyingSecurityDesc,
                Tag::EncodedUnderlyingSecurityDescLen, Tag::EncodedUnderlyingSecurityDesc,
                Tag::UnderlyingCPProgram, Tag::UnderlyingCPRegType, Tag::UnderlyingCurrency,
                Tag::UnderlyingQty, Tag::UnderlyingPx, Tag::UnderlyingDirtyPrice, Tag::UnderlyingEndPrice,
                Tag::UnderlyingStartValue, Tag::UnderlyingCurrentValue, Tag::UnderlyingEndValue,
                UnderlyingStipulations
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
                Tag::LegRedemptionDate, Tag::LegStrikePrice, Tag::LegStrikeCurrency, Tag::LegOptAttribute,
                Tag::LegContractMultiplier, Tag::LegCouponRate, Tag::LegSecurityExchange,
                Tag::LegIssuer, Tag::EncodedLegIssuerLen, Tag::EncodedLegIssuer, Tag::LegSecurityDesc,
                Tag::EncodedLegSecurityDescLen, Tag::EncodedLegSecurityDesc, Tag::LegRatioQty,
                Tag::LegSide, Tag::LegCurrency, Tag::LegPool, Tag::LegDatedDate, Tag::LegContractSettlMonth,
                Tag::LegInterestAccrualDate
            >;

        } // namespace Component

    } // namespace v44

} // namespace Fixpp
