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

    using SenderCompID = TagT<49, Type::String, Required>;
    using TargetCompID = TagT<56, Type::String, Required>;
    using OnBehalfOfCompID = TagT<115, Type::String>;
    using DeliverToCompID = TagT<128, Type::String>;
    using SecureDataLen = TagT<90, Type::Int>;
    using SecureData = TagT<91, Type::Data>;
    using SenderSubID = TagT<50, Type::String>;
    using SenderLocationID = TagT<142, Type::String>;
    using TargetSubID = TagT<57, Type::String>;
    using TargetLocationID = TagT<143, Type::String>;
    using OnBehalfOfSubID = TagT<116, Type::String>;
    using OnBehalfOfLocationID = TagT<144, Type::String>;
    using DeliverToSubID = TagT<129, Type::String>;
    using DeliverToLocationID = TagT<145, Type::String>;
    using PossDupFlag = TagT<43, Type::Boolean>;
    using PossResend = TagT<97, Type::Boolean>;
    using SendingTime = TagT<52, Type::UTCTimestamp, Required>;
    using OrigSendingTime = TagT<122, Type::UTCTimestamp>;
    using XmlDataLen = TagT<212, Type::Int>;
    using XmlData = TagT<213, Type::Data>;
    using MessageEncoding = TagT<347, Type::String>;
    using LastMsgSeqNumProcessed = TagT<369, Type::Int>;
    using OnBehalfOfSendingTime = TagT<370, Type::UTCTimestamp>; 

    using MsgSeqNum = TagT<34, Type::Int, Required>;

    using MsgType = TagT<35, Type::Char, Required>;
    using TestReqID = TagT<112, Type::String>;

    using EncryptMethod = TagT<98, Type::Int, Required>;
    using HeartBtInt = TagT<108, Type::Int, Required>;
    using ResetSeqNumFlag = TagT<141, Type::Int>;
    using MaxMessageSize = TagT<383, Type::Int>;
    using NoMsgTypes = TagT<384, Type::Int>;
    using RefMsgType = TagT<372, Type::String>;
    using MsgDirection = TagT<385, Type::Char>;

    using List = std::tuple<Account, AdvId, AdvRefID, AdvSide, AdvTransType, AvgPx, BeginSeqNo, BeginString, BodyLength, CheckSum,
                            SenderCompID, TargetCompID, OnBehalfOfCompID, DeliverToCompID, SecureDataLen, SecureData, SenderSubID,
                            SenderLocationID, TargetSubID, TargetLocationID, OnBehalfOfSubID, OnBehalfOfLocationID, PossDupFlag,
                            PossResend, SendingTime, OrigSendingTime, XmlDataLen, XmlData, MessageEncoding, LastMsgSeqNumProcessed,
                            OnBehalfOfSendingTime, MsgSeqNum, MsgType, TestReqID, EncryptMethod, HeartBtInt, ResetSeqNumFlag,
                            MaxMessageSize, NoMsgTypes, RefMsgType, MsgDirection>;

} // namespace Tag

    template<typename Tag> struct TagTraits
    {
        static constexpr unsigned Id = Tag::Id;
        static constexpr bool Required = Tag::Required;

        using Type = typename Tag::Type;
    };

} // namespace Fix

