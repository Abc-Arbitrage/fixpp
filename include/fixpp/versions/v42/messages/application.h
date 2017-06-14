/* application.h
   Mathieu Stefani, 13 june 2017
   
   Application messages definition for FIX 4.2
*/

#pragma once

#include <fixpp/message_type.h>
#include <fixpp/versions/base.h>
#include <fixpp/dsl.h>
#include <fixpp/versions/v42/version.h>

namespace Fixpp
{
	FIXPP_BEGIN_VERSION_NAMESPACE(v42, "FIX.4.2")
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
                  Tag::SpreadToBenchmark, Tag::Benchmark
            >;

            using Advertisement = MessageV<Chars<'7'>,
                  Required<Tag::AdvId>, Required<Tag::AdvTransType>, Tag::AdvRefID, Required<Tag::Symbol>, Tag::SymbolSfx,
                  Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall,
                  Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange,
                  Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen,
                  Tag::EncodedSecurityDesc, Required<Tag::AdvSide>, Required<Tag::Shares>, Tag::Price, Tag::Currency,
                  Tag::TradeDate, Tag::TransactTime, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText, Tag::URLLink,
                  Tag::LastMkt, Tag::TradingSessionID
            >;

            using ExecutionReport = MessageV<Chars<'8'>,
                  Required<Tag::OrderID>, Tag::SecondaryOrderID, Tag::ClOrdID, Tag::OrigClOrdID, Tag::ClientID, Tag::ExecBroker,
                  RepeatingGroup<
                      Tag::NoContraBrokers,
                      Tag::ContraBroker, Tag::ContraTrader, Tag::ContraTradeQty, Tag::ContraTradeTime
                  >, Tag::ListID, Required<Tag::ExecID>, Required<Tag::ExecTransType>, Tag::ExecRefID, Required<Tag::ExecType>,
                  Required<Tag::OrdStatus>, Tag::OrdRejReason, Tag::ExecRestatementReason, Tag::Account, Tag::SettlmntTyp, Tag::FutSettDate,
                  Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear,
                  Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate,
                  Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen,
                  Tag::EncodedSecurityDesc, Required<Tag::Side>, Tag::OrderQty, Tag::CashOrderQty, Tag::OrdType, Tag::Price,
                  Tag::StopPx, Tag::PegDifference, Tag::DiscretionInst, Tag::DiscretionOffset, Tag::Currency, Tag::ComplianceID,
                  Tag::SolicitedFlag, Tag::TimeInForce, Tag::EffectiveTime, Tag::ExpireDate, Tag::ExpireTime, Tag::ExecInst, Tag::Rule80A,
                  Tag::LastShares, Tag::LastPx, Tag::LastSpotRate, Tag::LastForwardPoints, Tag::LastMkt, Tag::TradingSessionID,
                  Tag::LastCapacity, Required<Tag::LeavesQty>, Required<Tag::CumQty>, Required<Tag::AvgPx>, Tag::DayOrderQty, Tag::DayCumQty,
                  Tag::DayAvgPx, Tag::GTBookingInst, Tag::TradeDate, Tag::TransactTime, Tag::ReportToExch, Tag::Commission, Tag::CommType,
                  Tag::GrossTradeAmt, Tag::SettlCurrAmt, Tag::SettlCurrency, Tag::SettlCurrFxRate, Tag::SettlCurrFxRateCalc, Tag::HandlInst,
                  Tag::MinQty, Tag::MaxFloor, Tag::OpenClose, Tag::MaxShow, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText, Tag::FutSettDate2,
                  Tag::OrderQty2, Tag::ClearingFirm, Tag::ClearingAccount, Tag::MultiLegReportingType
            >;

            using OrderCancelReject = MessageV<Chars<'9'>,
                  Required<Tag::OrderID>, Tag::SecondaryOrderID, Required<Tag::ClOrdID>, Required<Tag::OrigClOrdID>, Required<Tag::OrdStatus>,
                  Tag::ClientID, Tag::ExecBroker, Tag::ListID, Tag::Account, Tag::TransactTime, Required<Tag::CxlRejResponseTo>, Tag::CxlRejReason,
                  Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using News = MessageV<Chars<'B'>,
                  Tag::OrigTime, Tag::Urgency, Required<Tag::Headline>, Tag::EncodedHeadlineLen, Tag::EncodedHeadline,
                  RepeatingGroup<
                      Tag::NoRoutingIDs,
                      Tag::RoutingType, Tag::RoutingID
                  >,
                  RepeatingGroup<
                      Tag::NoRelatedSym,
                      Tag::RelatdSym, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear,
                      Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate,
                      Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                      Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc>,
                      Required<
                          RepeatingGroup<
                              Tag::LinesOfText,
                              Required<Tag::Text>, Tag::EncodedTextLen, Tag::EncodedText
                          >
                      >, Tag::URLLink, Tag::RawDataLength, Tag::RawData
            >;

            using Email = MessageV<Chars<'C'>,
                  Required<Tag::EmailThreadID>, Required<Tag::EmailType>, Tag::OrigTime, Required<Tag::Subject>, Tag::EncodedSubjectLen,
                  Tag::EncodedSubject, RepeatingGroup<Tag::NoRoutingIDs, Tag::RoutingType, Tag::RoutingID>,
                  RepeatingGroup<
                      Tag::NoRelatedSym,
                      Tag::RelatdSym, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay,
                      Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange,
                      Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc
                  >,
                  Tag::OrderID, Tag::ClOrdID, Required<RepeatingGroup<Tag::LinesOfText, Required<Tag::Text>, Tag::EncodedTextLen, Tag::EncodedText>>,
                  Tag::RawDataLength, Tag::RawData
            >;

            using NewOrderSingle = MessageV<Chars<'D'>,
                  Required<Tag::ClOrdID>, Tag::ClientID, Tag::ExecBroker, Tag::Account,
                  RepeatingGroup<Tag::NoAllocs,
                      Tag::AllocAccount, Tag::AllocShares
                  >, Tag::SettlmntTyp, Tag::FutSettDate, Required<Tag::HandlInst>, Tag::ExecInst, Tag::MinQty,
                  Tag::MaxFloor, Tag::ExDestination,
                  RepeatingGroup<Tag::NoTradingSessions, Tag::TradingSessionID>,
                  Tag::ProcessCode, Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID,
                  Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay,
                  Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier,
                  Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen,
                  Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc,
                  Tag::PrevClosePx, Required<Tag::Side>, Tag::LocateReqd, Required<Tag::TransactTime>,
                  Tag::OrderQty, Tag::CashOrderQty, Required<Tag::OrdType>, Tag::Price, Tag::StopPx,
                  Tag::Currency, Tag::ComplianceID, Tag::SolicitedFlag, Tag::IOIid, Tag::QuoteID,
                  Tag::TimeInForce, Tag::EffectiveTime, Tag::ExpireDate, Tag::ExpireTime, Tag::GTBookingInst,
                  Tag::Commission, Tag::CommType, Tag::Rule80A, Tag::ForexReq, Tag::SettlCurrency,
                  Tag::Text, Tag::EncodedTextLen, Tag::EncodedText, Tag::FutSettDate2, Tag::OrderQty2,
                  Tag::OpenClose, Tag::CoveredOrUncovered, Tag::CustomerOrFirm, Tag::MaxShow, Tag::PegDifference,
                  Tag::DiscretionInst, Tag::DiscretionOffset, Tag::ClearingFirm, Tag::ClearingAccount
            >;

            using NewOrderList = MessageV<Chars<'E'>,
                  Required<Tag::ListID>, Tag::BidID, Tag::ClientBidID, Tag::ProgRptReqs, Required<Tag::BidType>, Tag::ProgPeriodInterval,
                  Tag::ListExecInstType, Tag::ListExecInst, Tag::EncodedListExecInstLen, Tag::EncodedListExecInst, Required<Tag::TotNoOrders>,
                  Required<
                      RepeatingGroup<
                          Tag::NoOrders, Required<Tag::ClOrdID>, Required<Tag::ListSeqNo>, Tag::SettlInstMode, Tag::ClientID,
                          Tag::ExecBroker, Tag::Account,
                          RepeatingGroup<
                          Tag::NoAllocs,
                              Tag::AllocAccount, Tag::AllocShares
                          >, Tag::SettlmntTyp, Tag::FutSettDate, Tag::HandlInst, Tag::ExecInst, Tag::MinQty, Tag::MaxFloor, Tag::ExDestination,
                          RepeatingGroup<
                              Tag::NoTradingSessions,
                              Tag::TradingSessionID
                          >, Tag::ProcessCode, Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource,
                          Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute,
                          Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen,
                          Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Tag::PrevClosePx,
                          Required<Tag::Side>, Tag::SideValueInd, Tag::LocateReqd, Tag::TransactTime, Tag::OrderQty, Tag::CashOrderQty,
                          Tag::OrdType, Tag::Price, Tag::StopPx, Tag::Currency, Tag::ComplianceID, Tag::SolicitedFlag, Tag::IOIid, Tag::QuoteID,
                          Tag::TimeInForce, Tag::EffectiveTime, Tag::ExpireDate, Tag::ExpireTime, Tag::GTBookingInst, Tag::Commission,
                          Tag::CommType, Tag::Rule80A, Tag::ForexReq, Tag::SettlCurrency, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText,
                          Tag::FutSettDate2, Tag::OrderQty2, Tag::OpenClose, Tag::CoveredOrUncovered, Tag::CustomerOrFirm, Tag::MaxShow,
                          Tag::PegDifference, Tag::DiscretionInst, Tag::DiscretionOffset, Tag::ClearingFirm, Tag::ClearingAccount
                    >
                >
            >;

            using OrderCancelRequest = MessageV<Chars<'F'>,
                  Required<Tag::OrigClOrdID>, Tag::OrderID, Required<Tag::ClOrdID>, Tag::ListID, Tag::Account, Tag::ClientID, Tag::ExecBroker,
                  Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay,
                  Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer,
                  Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc,
                  Required<Tag::Side>, Required<Tag::TransactTime>, Tag::OrderQty, Tag::CashOrderQty, Tag::ComplianceID, Tag::SolicitedFlag,
                  Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using OrderCancelReplaceRequest = MessageV<Chars<'G'>,
                  Tag::OrderID, Tag::ClientID, Tag::ExecBroker, Required<Tag::OrigClOrdID>, Required<Tag::ClOrdID>, Tag::ListID, Tag::Account,
                  RepeatingGroup<
                      Tag::NoAllocs, Tag::AllocAccount, Tag::AllocShares
                  >, Tag::SettlmntTyp, Tag::FutSettDate, Required<Tag::HandlInst>, Tag::ExecInst, Tag::MinQty, Tag::MaxFloor, Tag::ExDestination,
                  RepeatingGroup<
                      Tag::NoTradingSessions, Tag::TradingSessionID
                  >,
                  Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay,
                  Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer,
                  Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Required<Tag::Side>,
                  Required<Tag::TransactTime>, Tag::OrderQty, Tag::CashOrderQty, Required<Tag::OrdType>, Tag::Price, Tag::StopPx, Tag::PegDifference,
                  Tag::DiscretionInst, Tag::DiscretionOffset, Tag::ComplianceID, Tag::SolicitedFlag, Tag::Currency, Tag::TimeInForce, Tag::EffectiveTime,
                  Tag::ExpireDate, Tag::ExpireTime, Tag::GTBookingInst, Tag::Commission, Tag::CommType, Tag::Rule80A, Tag::ForexReq, Tag::SettlCurrency,
                  Tag::Text, Tag::EncodedTextLen, Tag::EncodedText, Tag::FutSettDate2, Tag::OrderQty2, Tag::OpenClose, Tag::CoveredOrUncovered,
                  Tag::CustomerOrFirm, Tag::MaxShow, Tag::LocateReqd, Tag::ClearingFirm, Tag::ClearingAccount
            >;

            using OrderStatusRequest = MessageV<Chars<'H'>,
                  Tag::OrderID, Required<Tag::ClOrdID>, Tag::ClientID, Tag::Account, Tag::ExecBroker, Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID,
                  Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute,
                  Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                  Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Required<Tag::Side>
            >;

            using Allocation = MessageV<Chars<'J'>,
                  Required<Tag::AllocID>, Required<Tag::AllocTransType>, Tag::RefAllocID, Tag::AllocLinkID, Tag::AllocLinkType,
                  RepeatingGroup<
                      Tag::NoOrders,
                      Tag::ClOrdID, Tag::OrderID, Tag::SecondaryOrderID, Tag::ListID, Tag::WaveNo>, RepeatingGroup<Tag::NoExecs, Tag::LastShares,
                      Tag::ExecID, Tag::LastPx, Tag::LastCapacity
                  >, Required<Tag::Side>, Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType,
                  Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier,
                  Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                  Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Required<Tag::Shares>, Tag::LastMkt, Tag::TradingSessionID,
                  Required<Tag::AvgPx>, Tag::Currency, Tag::AvgPrxPrecision, Required<Tag::TradeDate>, Tag::TransactTime, Tag::SettlmntTyp,
                  Tag::FutSettDate, Tag::GrossTradeAmt, Tag::NetMoney, Tag::OpenClose, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText,
                  Tag::NumDaysInterest, Tag::AccruedInterestRate,
                  RepeatingGroup<
                      Tag::NoAllocs,
                      Tag::AllocAccount, Tag::AllocPrice, Required<Tag::AllocShares>, Tag::ProcessCode, Tag::BrokerOfCredit, Tag::NotifyBrokerOfCredit,
                      Tag::AllocHandlInst, Tag::AllocText, Tag::EncodedAllocTextLen, Tag::EncodedAllocText, Tag::ExecBroker, Tag::ClientID,
                      Tag::Commission, Tag::CommType, Tag::AllocAvgPx, Tag::AllocNetMoney, Tag::SettlCurrAmt, Tag::SettlCurrency, Tag::SettlCurrFxRate,
                      Tag::SettlCurrFxRateCalc, Tag::AccruedInterestAmt, Tag::SettlInstMode,
                      RepeatingGroup<
                          Tag::NoMiscFees,
                          Tag::MiscFeeAmt, Tag::MiscFeeCurr, Tag::MiscFeeType
                      >
                  >
            >;

            using ListCancelRequest = MessageV<Chars<'K'>,
                  Required<Tag::ListID>, Required<Tag::TransactTime>, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using ListExecute = MessageV<Chars<'L'>,
                  Required<Tag::ListID>, Tag::ClientBidID, Tag::BidID, Required<Tag::TransactTime>, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using ListStatusRequest = MessageV<Chars<'M'>,
                  Required<Tag::ListID>, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using ListStatus = MessageV<Chars<'N'>,
                  Required<Tag::ListID>, Required<Tag::ListStatusType>, Required<Tag::NoRpts>, Required<Tag::ListOrderStatus>, Required<Tag::RptSeq>,
                  Tag::ListStatusText, Tag::EncodedListStatusTextLen, Tag::EncodedListStatusText, Tag::TransactTime, Required<Tag::TotNoOrders>,
                  Required<
                      RepeatingGroup<
                          Tag::NoOrders,
                          Required<Tag::ClOrdID>, Required<Tag::CumQty>, Required<Tag::OrdStatus>, Required<Tag::LeavesQty>,
                          Required<Tag::CxlQty>, Required<Tag::AvgPx>, Tag::OrdRejReason, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
                      >
                  >
            >;

            using AllocationInstructionAck = MessageV<Chars<'P'>,
                  Tag::ClientID, Tag::ExecBroker, Required<Tag::AllocID>, Required<Tag::TradeDate>, Tag::TransactTime, Required<Tag::AllocStatus>,
                  Tag::AllocRejCode, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using DontKnowTrade = MessageV<Chars<'Q'>,
                  Required<Tag::OrderID>, Required<Tag::ExecID>, Required<Tag::DKReason>, Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID,
                  Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute,
                  Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer,
                  Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Required<Tag::Side>, Tag::OrderQty, Tag::CashOrderQty,
                  Tag::LastShares, Tag::LastPx, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using QuoteRequest = MessageV<Chars<'R'>,
                  Required<Tag::QuoteReqID>,
                  Required<
                      RepeatingGroup<
                          Tag::NoRelatedSym,
                          Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear,
                          Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate,
                          Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen,
                          Tag::EncodedSecurityDesc, Tag::PrevClosePx, Tag::QuoteRequestType, Tag::TradingSessionID, Tag::Side, Tag::OrderQty,
                          Tag::FutSettDate, Tag::OrdType, Tag::FutSettDate2, Tag::OrderQty2, Tag::ExpireTime, Tag::TransactTime, Tag::Currency
                      >
                  >
            >;

            using Quote = MessageV<Chars<'S'>,
                Tag::QuoteReqID, Required<Tag::QuoteID>, Tag::QuoteResponseLevel, Tag::TradingSessionID,
                Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType,
                Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute,
                Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen,
                Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc,
                Tag::BidPx, Tag::OfferPx, Tag::BidSize, Tag::OfferSize, Tag::ValidUntilTime, Tag::BidSpotRate,
                Tag::OfferSpotRate, Tag::BidForwardPoints, Tag::OfferForwardPoints, Tag::TransactTime,
                Tag::FutSettDate, Tag::OrdType, Tag::FutSettDate2, Tag::OrderQty2, Tag::Currency
            >;

            using SettlementInstructions = MessageV<Chars<'T'>,
                  Required<Tag::SettlInstID>, Required<Tag::SettlInstTransType>, Required<Tag::SettlInstRefID>, Required<Tag::SettlInstMode>,
                  Required<Tag::SettlInstSource>, Required<Tag::AllocAccount>, Tag::SettlLocation, Tag::TradeDate, Tag::AllocID,
                  Tag::LastMkt, Tag::TradingSessionID, Tag::Side, Tag::SecurityType, Tag::EffectiveTime, Required<Tag::TransactTime>,
                  Tag::ClientID, Tag::ExecBroker, Tag::StandInstDbType, Tag::StandInstDbName, Tag::StandInstDbID, Tag::SettlDeliveryType,
                  Tag::SettlDepositoryCode, Tag::SettlBrkrCode, Tag::SettlInstCode, Tag::SecuritySettlAgentName, Tag::SecuritySettlAgentCode,
                  Tag::SecuritySettlAgentAcctNum, Tag::SecuritySettlAgentAcctName, Tag::SecuritySettlAgentContactName,
                  Tag::SecuritySettlAgentContactPhone, Tag::CashSettlAgentName, Tag::CashSettlAgentCode, Tag::CashSettlAgentAcctNum,
                  Tag::CashSettlAgentAcctName, Tag::CashSettlAgentContactName, Tag::CashSettlAgentContactPhone
            >;

            using MarketDataRequest = MessageV<Chars<'V'>,
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

            using MarketDataSnapshot = MessageV<Chars<'W'>,
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

            using MarketDataIncrementalRefresh = MessageV<Chars<'X'>,
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

            using MarketDataRequestReject = MessageV<Chars<'Y'>,
                  Required<Tag::MDReqID>, Tag::MDReqRejReason, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using QuoteCancel = MessageV<Chars<'Z'>,
                  Tag::QuoteReqID, Required<Tag::QuoteID>, Required<Tag::QuoteCancelType>, Tag::QuoteResponseLevel,
                  Tag::TradingSessionID,
                  Required<
                      RepeatingGroup<
                          Tag::NoQuoteEntries,
                          Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource,
                          Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice,
                          Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer,
                          Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen,
                          Tag::EncodedSecurityDesc, Tag::UnderlyingSymbol
                      >
                  >
            >;

            using QuoteStatusRequest = MessageV<Chars<'a'>,
                  Tag::QuoteID, Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType,
                  Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute,
                  Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen,
                  Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc,
                  Tag::Side, Tag::TradingSessionID
            >;

            using QuoteAcknowledgement = MessageV<Chars<'b'>,
                  Tag::QuoteReqID, Tag::QuoteID, Required<Tag::QuoteAckStatus>, Tag::QuoteRejectReason, Tag::QuoteResponseLevel,
                  Tag::TradingSessionID, Tag::Text,
                  RepeatingGroup<
                      Tag::NoQuoteSets,
                      Tag::QuoteSetID, Tag::UnderlyingSymbol, Tag::UnderlyingSymbolSfx, Tag::UnderlyingSecurityID, Tag::UnderlyingIDSource,
                      Tag::UnderlyingSecurityType, Tag::UnderlyingMaturityMonthYear, Tag::UnderlyingMaturityDay, Tag::UnderlyingPutOrCall,
                      Tag::UnderlyingStrikePrice, Tag::UnderlyingOptAttribute, Tag::UnderlyingContractMultiplier, Tag::UnderlyingCouponRate,
                      Tag::UnderlyingSecurityExchange, Tag::UnderlyingIssuer, Tag::EncodedUnderlyingIssuerLen, Tag::EncodedUnderlyingIssuer,
                      Tag::UnderlyingSecurityDesc, Tag::EncodedUnderlyingSecurityDescLen, Tag::EncodedUnderlyingSecurityDesc, Tag::TotQuoteEntries,
                      RepeatingGroup<
                          Tag::NoQuoteEntries,
                          Tag::QuoteEntryID, Tag::Symbol, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType,
                          Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute,
                          Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen,
                          Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Tag::QuoteEntryRejectReason
                      >
                  >
            >;

            using SecurityDefinitionRequest = MessageV<Chars<'c'>,
                  Required<Tag::SecurityReqID>, Required<Tag::SecurityRequestType>, Tag::Symbol, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource,
                  Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier,
                  Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                  Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Tag::Currency, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText,
                  Tag::TradingSessionID,
                  RepeatingGroup<
                      Tag::NoRelatedSym,
                      Tag::UnderlyingSymbol, Tag::UnderlyingSymbolSfx, Tag::UnderlyingSecurityID, Tag::UnderlyingIDSource, Tag::UnderlyingSecurityType,
                      Tag::UnderlyingMaturityMonthYear, Tag::UnderlyingMaturityDay, Tag::UnderlyingPutOrCall, Tag::UnderlyingStrikePrice,
                      Tag::UnderlyingOptAttribute, Tag::UnderlyingContractMultiplier, Tag::UnderlyingCouponRate, Tag::UnderlyingSecurityExchange,
                      Tag::UnderlyingIssuer, Tag::EncodedUnderlyingIssuerLen, Tag::EncodedUnderlyingIssuer, Tag::UnderlyingSecurityDesc,
                      Tag::EncodedUnderlyingSecurityDescLen, Tag::EncodedUnderlyingSecurityDesc, Tag::RatioQty, Tag::Side, Tag::UnderlyingCurrency
                  >
            >;

            using SecurityDefinition = MessageV<Chars<'d'>,
                  Required<Tag::SecurityReqID>, Required<Tag::SecurityResponseID>, Tag::SecurityResponseType, Required<Tag::TotalNumSecurities>,
                  Tag::Symbol, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay,
                  Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange,
                  Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc,
                  Tag::Currency, Tag::TradingSessionID, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText,
                  RepeatingGroup<
                      Tag::NoRelatedSym,
                      Tag::UnderlyingSymbol, Tag::UnderlyingSymbolSfx, Tag::UnderlyingSecurityID, Tag::UnderlyingIDSource, Tag::UnderlyingSecurityType,
                      Tag::UnderlyingMaturityMonthYear, Tag::UnderlyingMaturityDay, Tag::UnderlyingPutOrCall, Tag::UnderlyingStrikePrice,
                      Tag::UnderlyingOptAttribute, Tag::UnderlyingContractMultiplier, Tag::UnderlyingCouponRate, Tag::UnderlyingSecurityExchange,
                      Tag::UnderlyingIssuer, Tag::EncodedUnderlyingIssuerLen, Tag::EncodedUnderlyingIssuer, Tag::UnderlyingSecurityDesc,
                      Tag::EncodedUnderlyingSecurityDescLen, Tag::EncodedUnderlyingSecurityDesc, Tag::RatioQty, Tag::Side, Tag::UnderlyingCurrency
                  >
            >;

            using SecurityStatusRequest = MessageV<Chars<'e'>,
                Required<Tag::SecurityStatusReqID>, Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType,
                Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier,
                Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Tag::Currency, Required<Tag::SubscriptionRequestType>, Tag::TradingSessionID
            >;

            using SecurityStatus = MessageV<Chars<'f'>,
                  Tag::SecurityStatusReqID, Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType,
                  Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier,
                  Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                  Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Tag::Currency, Tag::TradingSessionID, Tag::UnsolicitedIndicator,
                  Tag::SecurityTradingStatus, Tag::FinancialStatus, Tag::CorporateAction, Tag::HaltReasonChar, Tag::InViewOfCommon,
                  Tag::DueToRelated, Tag::BuyVolume, Tag::SellVolume, Tag::HighPx, Tag::LowPx, Tag::LastPx, Tag::TransactTime, Tag::Adjustment
            >;

            using TradingSessionStatusRequest = MessageV<Chars<'g'>,
                  Required<Tag::TradSesReqID>, Tag::TradingSessionID, Tag::TradSesMethod, Tag::TradSesMode, Required<Tag::SubscriptionRequestType>
            >;

            using TradingSessionStatus = MessageV<Chars<'h'>,
                  Tag::TradSesReqID, Required<Tag::TradingSessionID>, Tag::TradSesMethod, Tag::TradSesMode, Tag::UnsolicitedIndicator,
                  Required<Tag::TradSesStatus>, Tag::TradSesStartTime, Tag::TradSesOpenTime, Tag::TradSesPreCloseTime, Tag::TradSesCloseTime,
                  Tag::TradSesEndTime, Tag::TotalVolumeTraded, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using MassQuote = MessageV<Chars<'i'>,
                  Tag::QuoteReqID, Required<Tag::QuoteID>, Tag::QuoteResponseLevel, Tag::DefBidSize, Tag::DefOfferSize,
                  Required<
                      RepeatingGroup<
                          Tag::NoQuoteSets, Required<Tag::QuoteSetID>, Required<Tag::UnderlyingSymbol>, Tag::UnderlyingSymbolSfx, Tag::UnderlyingSecurityID,
                          Tag::UnderlyingIDSource, Tag::UnderlyingSecurityType, Tag::UnderlyingMaturityMonthYear, Tag::UnderlyingMaturityDay, Tag::UnderlyingPutOrCall,
                          Tag::UnderlyingStrikePrice, Tag::UnderlyingOptAttribute, Tag::UnderlyingContractMultiplier, Tag::UnderlyingCouponRate,
                          Tag::UnderlyingSecurityExchange, Tag::UnderlyingIssuer, Tag::EncodedUnderlyingIssuerLen, Tag::EncodedUnderlyingIssuer,
                          Tag::UnderlyingSecurityDesc, Tag::EncodedUnderlyingSecurityDescLen, Tag::EncodedUnderlyingSecurityDesc, Tag::QuoteSetValidUntilTime,
                          Required<Tag::TotQuoteEntries>,
                          Required<
                              RepeatingGroup<
                                  Tag::NoQuoteEntries, Required<Tag::QuoteEntryID>, Tag::Symbol, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType,
                                  Tag::MaturityMonthYear, Tag::MaturityDay, Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier,
                                  Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer, Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc,
                                  Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc, Tag::BidPx, Tag::OfferPx, Tag::BidSize, Tag::OfferSize, Tag::ValidUntilTime,
                                  Tag::BidSpotRate, Tag::OfferSpotRate, Tag::BidForwardPoints, Tag::OfferForwardPoints, Tag::TransactTime, Tag::TradingSessionID,
                                  Tag::FutSettDate, Tag::OrdType, Tag::FutSettDate2, Tag::OrderQty2, Tag::Currency
                              >
                          >
                      >
                  >
            >;

            using BusinessMessageReject = MessageV<Chars<'j'>,
                  Tag::RefSeqNum, Required<Tag::RefMsgType>, Tag::BusinessRejectRefID, Required<Tag::BusinessRejectReason>, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using BidRequest = MessageV<Chars<'k'>,
                  Tag::BidID, Required<Tag::ClientBidID>, Required<Tag::BidRequestTransType>, Tag::ListName, Required<Tag::TotalNumSecurities>, Required<Tag::BidType>,
                  Tag::NumTickets, Tag::Currency, Tag::SideValue1, Tag::SideValue2, RepeatingGroup<Tag::NoBidDescriptors, Tag::BidDescriptorType, Tag::BidDescriptor,
                  Tag::SideValueInd, Tag::LiquidityValue, Tag::LiquidityNumSecurities, Tag::LiquidityPctLow, Tag::LiquidityPctHigh, Tag::EFPTrackingError, Tag::FairValue,
                  Tag::OutsideIndexPct, Tag::ValueOfFutures>, RepeatingGroup<Tag::NoBidComponents, Tag::ListID, Tag::Side, Tag::TradingSessionID, Tag::NetGrossInd,
                  Tag::SettlmntTyp, Tag::FutSettDate, Tag::Account>, Tag::LiquidityIndType, Tag::WtAverageLiquidity, Tag::ExchangeForPhysical, Tag::OutMainCntryUIndex,
                  Tag::CrossPercent, Tag::ProgRptReqs, Tag::ProgPeriodInterval, Tag::IncTaxInd, Tag::ForexReq, Tag::NumBidders, Tag::TradeDate, Required<Tag::TradeType>,
                  Required<Tag::BasisPxType>, Tag::StrikeTime, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
            >;

            using BidResponse = MessageV<Chars<'l'>,
                  Tag::BidID, Tag::ClientBidID,
                  Required<
                      RepeatingGroup<
                          Tag::NoBidComponents,
                          Required<Tag::Commission>, Required<Tag::CommType>, Tag::ListID, Tag::Country, Tag::Side, Tag::Price, Tag::PriceType,
                          Tag::FairValue, Tag::NetGrossInd, Tag::SettlmntTyp, Tag::FutSettDate, Tag::TradingSessionID, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
                      >
                  >
            >;

            using ListStrikePrice = MessageV<Chars<'m'>,
                  Required<Tag::ListID>, Required<Tag::TotNoStrikes>,
                  Required<
                      RepeatingGroup<
                          Tag::NoStrikes,
                          Required<Tag::Symbol>, Tag::SymbolSfx, Tag::SecurityID, Tag::IDSource, Tag::SecurityType, Tag::MaturityMonthYear, Tag::MaturityDay,
                          Tag::PutOrCall, Tag::StrikePrice, Tag::OptAttribute, Tag::ContractMultiplier, Tag::CouponRate, Tag::SecurityExchange, Tag::Issuer,
                          Tag::EncodedIssuerLen, Tag::EncodedIssuer, Tag::SecurityDesc, Tag::EncodedSecurityDescLen, Tag::EncodedSecurityDesc,
                          Tag::PrevClosePx, Tag::ClOrdID, Tag::Side, Required<Tag::Price>, Tag::Currency, Tag::Text, Tag::EncodedTextLen, Tag::EncodedText
                      >
                  >
            >;
		} // namespace Message
		
		namespace Spec
		{
			struct Application
			{
				using IndicationOfIntereset = Message::IndicationOfInterest;
				using Advertisement = Message::Advertisement;
				using ExecutionReport = Message::ExecutionReport;
				using OrderCancelReject = Message::OrderCancelReject;
				using News = Message::News;
				using Email = Message::Email;
				using NewOrderSingle = Message::NewOrderSingle;
				using NewOrderList = Message::NewOrderList;
				using OrderCancelRequest = Message::OrderCancelRequest;
				using OrderCancelReplaceRequest = Message::OrderCancelReplaceRequest;
				using OrderStatusRequest = Message::OrderStatusRequest;
				using Allocation = Message::Allocation;
				using ListCancelRequest = Message::ListCancelRequest;
				using ListExecute = Message::ListExecute;
				using ListStatusRequest = Message::ListStatusRequest;
				using ListStatus = Message::ListStatus;
				using AllocationInstructionAck = Message::AllocationInstructionAck;
				using DontKnowTrade = Message::DontKnowTrade;
				using QuoteRequest = Message::QuoteRequest;
				using Quote = Message::Quote;
				using SettlementInstructions = Message::SettlementInstructions;
				using MarketDataRequest = Message::MarketDataRequest;
				using MarketDataSnapshot = Message::MarketDataSnapshot;
				using MarketDataIncrementalRefresh = Message::MarketDataIncrementalRefresh;
				using MarketDataRequestReject = Message::MarketDataRequestReject;
				using QuoteCancel = Message::QuoteCancel;
				using QuoteStatusRequest = Message::QuoteStatusRequest;
				using MassQuote = Message::MassQuote;
				using BusinessMessageReject = Message::BusinessMessageReject;
				using BidRequest = Message::BidRequest;
				using BidResponse = Message::BidResponse;
				using ListStrikePrice = Message::ListStrikePrice;
			};
		} // namespace Spec
	}
	FIXPP_END_VERSION_NAMESPACE
} // namespace Fixpp