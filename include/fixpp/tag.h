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

    } // namespace Type

    enum Flag
    {
        Required,
        Optional
    };

    template<unsigned N, typename T, Flag flag = Flag::Optional> struct TagT
    {
        static constexpr unsigned Id = N;

        using Type = T;

        static constexpr bool Required = flag == Flag::Required;

        constexpr bool isRequired() const
        {
            return Required;
        }
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
        using BeginString = TagT<8, Type::String, Required>;
        using BodyLength = TagT<9, Type::Int, Required>;
        using CheckSum = TagT<10, Type::String, Required>;
        using Currency = TagT<15, Type::String>;
        using EndSeqNo = TagT<16, Type::Int, Required>;
        using IDSource = TagT<22, Type::String>;
        using IOIid = TagT<23, Type::String, Required>;
        using IOIQltyInd = TagT<25, Type::Char>;
        using IOIRefID = TagT<26, Type::String>;
        using IOIShares = TagT<27, Type::String, Required>;
        using IOITransType = TagT<28, Type::Char, Required>;

        using MsgSeqNum = TagT<34, Type::Int, Required>;
        using MsgType = TagT<35, Type::Char, Required>;
        using NewSeqNo = TagT<36, Type::Int, Required>;
        using PossDupFlag = TagT<43, Type::Boolean>;
        using Price = TagT<44, Type::Float>;
        using RefSeqNum = TagT<45, Type::Int, Required>;
        using SecurityID = TagT<48, Type::String>;
        using SenderCompID = TagT<49, Type::String, Required>;
        using SenderSubID = TagT<50, Type::String>;
        using SendingTime = TagT<52, Type::UTCTimestamp, Required>;
        using Side = TagT<54, Type::Char, Required>;
        using Symbol = TagT<55, Type::String, Required>;
        using TargetCompID = TagT<56, Type::String, Required>;
        using TargetSubID = TagT<57, Type::String>;
        using Text = TagT<58, Type::String>;
        using TransactTime = TagT<60, Type::UTCTimestamp>;
        using ValidUntilTime = TagT<62, Type::UTCTimestamp>;
        using SymbolSfx = TagT<65, Type::String>;
        using SecureDataLen = TagT<90, Type::Int>;
        using SecureData = TagT<91, Type::Data>;
        using RawDataLength = TagT<95, Type::Int>;
        using RawData = TagT<96, Type::Data>;
        using PossResend = TagT<97, Type::Boolean>;
        using EncryptMethod = TagT<98, Type::Int, Required>;
        using IOIQualifier = TagT<104, Type::Char>;
        using Issuer = TagT<106, Type::String>;
        using SecurityDesc = TagT<107, Type::String>;
        using HeartBtInt = TagT<108, Type::Int, Required>;
        using TestReqID = TagT<112, Type::String>;
        using OnBehalfOfCompID = TagT<115, Type::String>;
        using OnBehalfOfSubID = TagT<116, Type::String>;
        using OrigSendingTime = TagT<122, Type::UTCTimestamp>;
        using GapFillFlag = TagT<123, Type::Boolean>;
        using DeliverToCompID = TagT<128, Type::String>;
        using DeliverToSubID = TagT<129, Type::String>;
        using IOINaturalFlag = TagT<130, Type::Boolean>;
        using ResetSeqNumFlag = TagT<141, Type::Boolean>;
        using SenderLocationID = TagT<142, Type::String>;
        using TargetLocationID = TagT<143, Type::String>;
        using OnBehalfOfLocationID = TagT<144, Type::String>;
        using DeliverToLocationID = TagT<145, Type::String>;
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
        using MessageEncoding = TagT<347, Type::String>;
        using EncodedSecurityDescLen = TagT<350, Type::Int>;
        using EncodedSecurityDesc = TagT<351, Type::Data>;
        using EncodedTextLen = TagT<354, Type::Int>;
        using EncodedText = TagT<355, Type::Data>;
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

    } // namespace Tag

    template<typename Tag> struct TagTraits
    {
        static constexpr unsigned Id = Tag::Id;
        static constexpr bool Required = Tag::Required;

        using Type = typename Tag::Type;
    };

} // namespace Fix

