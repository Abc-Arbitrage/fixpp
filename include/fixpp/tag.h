/* tag.h
   Mathieu Stefani, 12 november 2016
   
   A typed list of FIX tags
*/

#pragma once

#include <string>
#include <iostream>
#include <ctime>
#include <tuple>

namespace Fix
{
    namespace Type
    {
        template<typename T>
        struct Base
        {
            using UnderlyingType = T;

            Base() { }

            Base(T);
        };

        struct Char : public Base<char>
        {
            using Base::Base;
        };

        struct Boolean
        {
            struct Boxed
            {
                Boxed(bool value = false)
                    : m_value(value)
                { }

                Boxed(const Boxed& other) = default;
                Boxed(Boxed&& other) = default;

                Boxed& operator=(const Boxed& other) = default;
                Boxed& operator=(Boxed&& other) = default;

                operator bool() const
                {
                    return m_value;
                }

            private:
                bool m_value;
            };

            using UnderlyingType = Boxed;

            Boolean(bool);
            Boolean(const Boxed&);
        };

        struct Float : public Base<float>
        {
            using Base::Base;
        };

        using Amt = Float;
        using Price = Float;
        using Qty = Float;

        struct Int : public Base<int>
        {
            using Base::Base;
        };

        struct Data
        {
            using UnderlyingType = std::string;

            Data(const std::string&);
            Data(const char *);
        };

        struct String : public Base<std::string>
        {
            using Base::Base;

            String(const char*);
        };

        using MultipleValueString = Type::String;

        using DayOfMonth = Int;
        using MonthYear = String;

        struct UTCTimestamp
        {
            struct Time
            {
                Time()
                    : m_time(std::time(nullptr))
                { }

                Time(const std::time_t& time)
                    : m_time(time)
                { }

                std::time_t time() const
                {
                    return m_time;
                }

            private:
                std::time_t m_time;
            };

            using UnderlyingType = Time;

            UTCTimestamp(const std::string &);
            UTCTimestamp(const Time&);
            UTCTimestamp(const std::time_t&);
        };

        std::ostream& operator<<(std::ostream& os, const UTCTimestamp::Time& value)
        {
            auto time = value.time();
            char buffer[32];
            strftime(buffer, sizeof buffer, "%Y%m%d-%H%M%S", std::gmtime(&time)); 
            os << buffer;
            return os;
        }

        std::ostream& operator<<(std::ostream& os, const Boolean::Boxed& value)
        {
            os << (value ? 'Y' : 'N');
            return os;
        }

        using UTCDate = String;
        using UTCTimeOnly = String;

    } // namespace Type

    template<unsigned N, typename T> struct TagT
    {
        static constexpr unsigned Id = N;

        using Type = T;
    };


    namespace Tag
    {
        using Account = TagT<1, Type::String>;
        using AdvId = TagT<2, Type::String>;
        using AdvRefID = TagT<3, Type::String>;
        using AdvSide = TagT<4, Type::Char>;
        using AdvTransType = TagT<5, Type::String>;
        using AvgPx = TagT<6, Type::Float>;
        using BeginSeqNo = TagT<7, Type::Int>;
        using BeginString = TagT<8, Type::String>;
        using BodyLength = TagT<9, Type::Int>;
        using CheckSum = TagT<10, Type::String>;
        using ClOrdID = TagT<11, Type::String>;
        using Commission = TagT<12, Type::Amt>;
        using CommType  = TagT<13, Type::Char>;
        using CumQty = TagT<14, Type::Qty>;
        using Currency = TagT<15, Type::String>;
        using EndSeqNo = TagT<16, Type::Int>;
        using ExecID  = TagT<17, Type::String>;
        using ExecInst = TagT<18, Type::MultipleValueString>;
        using ExecRefID = TagT<19, Type::String>;
        using ExecTransType = TagT<20, Type::Char>;
        using HandlInst = TagT<21, Type::Char>;
        using IDSource = TagT<22, Type::String>;
        using IOIid = TagT<23, Type::String>;

        using IOIQltyInd = TagT<25, Type::Char>;
        using IOIRefID = TagT<26, Type::String>;
        using IOIShares = TagT<27, Type::String>;
        using IOITransType = TagT<28, Type::Char>;
        using LastCapacity = TagT<29, Type::Char>;
        using LastMkt = TagT<30, Type::String>;
        using LastPx = TagT<31, Type::Price>;
        using LastShares = TagT<32, Type::Qty>;
        using LinesOfText = TagT<33, Type::Int>;
        using MsgSeqNum = TagT<34, Type::Int>;
        using MsgType = TagT<35, Type::Char>;
        using NewSeqNo = TagT<36, Type::Int>;
        using OrderID = TagT<37, Type::String>;
        using OrderQty = TagT<38, Type::Qty>;
        using OrdStatus = TagT<39, Type::Char>;
        using OrdType = TagT<40, Type::Char>;
        using OrigClOrdID = TagT<41, Type::String>;
        using OrigTime  = TagT<42, Type::UTCTimestamp>;
        using PossDupFlag = TagT<43, Type::Boolean>;
        using Price = TagT<44, Type::Float>;
        using RefSeqNum = TagT<45, Type::Int>;
        using SecurityID = TagT<48, Type::String>;
        using SenderCompID = TagT<49, Type::String>;
        using SenderSubID = TagT<50, Type::String>;
        using SendingTime = TagT<52, Type::UTCTimestamp>;
        using Side = TagT<54, Type::Char>;
        using Symbol = TagT<55, Type::String>;
        using TargetCompID = TagT<56, Type::String>;
        using TargetSubID = TagT<57, Type::String>;
        using Text = TagT<58, Type::String>;
        using TimeInForce = TagT<59, Type::Char>;
        using TransactTime = TagT<60, Type::UTCTimestamp>;
        using ValidUntilTime = TagT<62, Type::UTCTimestamp>;
        using SymbolSfx = TagT<65, Type::String>;
        using SecureDataLen = TagT<90, Type::Int>;
        using SecureData = TagT<91, Type::Data>;
        using RawDataLength = TagT<95, Type::Int>;
        using RawData = TagT<96, Type::Data>;
        using PossResend = TagT<97, Type::Boolean>;
        using EncryptMethod = TagT<98, Type::Int>;
        using IOIQualifier = TagT<104, Type::Char>;
        using Issuer = TagT<106, Type::String>;
        using SecurityDesc = TagT<107, Type::String>;
        using HeartBtInt = TagT<108, Type::Int>;
        using MinQty = TagT<110, Type::Qty>;
        using TestReqID = TagT<112, Type::String>;
        using OnBehalfOfCompID = TagT<115, Type::String>;
        using OnBehalfOfSubID = TagT<116, Type::String>;
        using OrigSendingTime = TagT<122, Type::UTCTimestamp>;
        using GapFillFlag = TagT<123, Type::Boolean>;
        using ExpireTime = TagT<126, Type::UTCTimestamp>;
        using DeliverToCompID = TagT<128, Type::String>;
        using DeliverToSubID = TagT<129, Type::String>;
        using IOINaturalFlag = TagT<130, Type::Boolean>;
        using ResetSeqNumFlag = TagT<141, Type::Boolean>;
        using SenderLocationID = TagT<142, Type::String>;
        using TargetLocationID = TagT<143, Type::String>;
        using OnBehalfOfLocationID = TagT<144, Type::String>;
        using DeliverToLocationID = TagT<145, Type::String>;
        using NoRelatedSym = TagT<146, Type::Int>;
        using URLLink = TagT<149, Type::String>;
        using SecurityType = TagT<167, Type::String>;
        using NoIOIQualifiers = TagT<199, Type::Int>;
        using MaturityMonthYear = TagT<200, Type::MonthYear>;
        using PutOrCall = TagT<201, Type::Int>;
        using StrikePrice = TagT<202, Type::Float>;
        using MaturityDay = TagT<205, Type::DayOfMonth>;
        using OptAttribute = TagT<206, Type::Char>;
        using SecurityExchange = TagT<207, Type::String>;
        using XmlDataLen = TagT<212, Type::Int>;
        using XmlData = TagT<213, Type::Data>;
        using NoRoutingIDs = TagT<215, Type::Int>;
        using RoutingType = TagT<216, Type::Int>;
        using RoutingID = TagT<217, Type::String>;
        using SpreadToBenchmark = TagT<218, Type::Float>;
        using Benchmark = TagT<219, Type::Char>;
        using CouponRate = TagT<223, Type::Float>;
        using ContractMultiplier = TagT<231, Type::Float>;
        using MDReqID = TagT<262, Type::String>;
        using SubscriptionRequestType = TagT<263, Type::Char>;
        using MarketDepth = TagT<264, Type::Int>;
        using MDUpdateType = TagT<265, Type::Int>;
        using AggregatedBook = TagT<266, Type::Boolean>;
        using NoMDEntryTypes = TagT<267, Type::Int>;
        using NoMDEntries = TagT<268, Type::Int>;
        using MDEntryType = TagT<269, Type::Char>;
        using MDEntryPx = TagT<270, Type::Price>;
        using MDEntrySize = TagT<271, Type::Qty>;
        using MDEntryDate = TagT<272, Type::UTCDate>;
        using MDEntryTime = TagT<273, Type::UTCTimeOnly>;
        using TickDirection = TagT<274, Type::Char>;
        using MDMkt = TagT<275, Type::String>;
        using QuoteCondition = TagT<276, Type::MultipleValueString>;
        using TradeCondition = TagT<277, Type::MultipleValueString>;
        using MDEntryID = TagT<278, Type::String>;
        using MDUpdateAction = TagT<279, Type::Char>;
        using MDEntryRefID = TagT<280, Type::String>;
        using MDEntryOriginator = TagT<282, Type::String>;
        using LocationID = TagT<283, Type::String>;
        using DeskID = TagT<284, Type::String>;
        using DeleteReason = TagT<285, Type::Char>;
        using OpenCloseSettleFlag = TagT<286, Type::Char>;
        using SellerDays = TagT<287, Type::Int>;
        using MDEntryBuyer = TagT<288, Type::String>;
        using MDEntrySeller = TagT<289, Type::String>;
        using MDEntryPositionNo = TagT<290, Type::Int>;
        using FinancialStatus = TagT<291, Type::Char>;
        using CorporateAction = TagT<292, Type::Char>;
        using QuoteEntryID = TagT<299, Type::String>;
        using TradingSessionID = TagT<336, Type::String>;
        using MessageEncoding = TagT<347, Type::String>;
        using EncodedSecurityDescLen = TagT<350, Type::Int>;
        using EncodedSecurityDesc = TagT<351, Type::Data>;
        using EncodedTextLen = TagT<354, Type::Int>;
        using EncodedText = TagT<355, Type::Data>;
        using NumberOfOrders = TagT<346, Type::Int>;
        using EncodedIssuerLen = TagT<348, Type::Int>;
        using EncodedIssuer = TagT<349, Type::Data>;
        using LastMsgSeqNumProcessed = TagT<369, Type::Int>;
        using OnBehalfOfSendingTime = TagT<370, Type::UTCTimestamp>; 
        using RefTagID = TagT<371, Type::Int>;
        using RefMsgType = TagT<372, Type::String>;
        using SessionRejectReason = TagT<373, Type::Int>;

        using MaxMessageSize = TagT<383, Type::Int>;
        using NoMsgTypes = TagT<384, Type::Int>;
        using MsgDirection = TagT<385, Type::Char>;
        using TotalVolumeTraded = TagT<387, Type::Qty>;
        using ExpireDate = TagT<432, Type::String>;

    } // namespace Tag

    template<typename Tag> struct TagTraits
    {
        static constexpr unsigned Id = Tag::Id;
        static constexpr bool Required = Tag::Required;

        using Type = typename Tag::Type;
    };

} // namespace Fix

