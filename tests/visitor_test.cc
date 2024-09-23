#include "gtest/gtest.h"

#define SOH_CHARACTER '|'

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v44.h>
#include <fixpp/visitor.h>
#include <fixpp/dsl/message.h>

template<typename Dict>
struct DefaultTestRules : public Fixpp::VisitRules
{
    using Overrides = OverrideSet<>;
    using Dictionary = Dict;

    static constexpr bool ValidateChecksum = true;
    static constexpr bool ValidateLength = true;
    static constexpr bool StrictMode = false;
    static constexpr bool SkipUnknownTags = false;
};

namespace should_visit_logon_frame
{
    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v42::Header::Ref& header, const Fixpp::v42::Message::Logon::Ref& logon)
        {
            ASSERT_EQ(Fixpp::get<Fixpp::Tag::SenderCompID>(header), "ABC");
            ASSERT_EQ(Fixpp::get<Fixpp::Tag::HeartBtInt>(logon), 60);

            Fixpp::get<Fixpp::Tag::SendingTime>(header);

            ASSERT_THROW(Fixpp::get<Fixpp::Tag::MaxMessageSize>(logon), std::runtime_error);
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

    using VisitRules = DefaultTestRules<Fixpp::v42::Spec::Dictionary>;

} // namespace should_visit_logon_frame

namespace should_visit_message_with_multiple_chars_message_type
{
    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v44::Header::Ref& /*header*/, const Fixpp::v44::Message::UserResponse::Ref& response)
        {
            ASSERT_EQ(Fixpp::get<Fixpp::Tag::UserRequestID>(response), "123");
            ASSERT_EQ(Fixpp::get<Fixpp::Tag::UserRequestType>(response), 1);
            ASSERT_EQ(Fixpp::get<Fixpp::Tag::Username>(response), "username");
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

    using VisitRules = DefaultTestRules<Fixpp::v44::Spec::Dictionary>;

} // namespace should_visit_logon_frame

namespace should_try_get_fields_after_parsing
{
    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v42::Header::Ref& header, const Fixpp::v42::Message::Logon::Ref& logon)
        {
            std::string senderCompId;
            ASSERT_TRUE(Fixpp::tryGet<Fixpp::Tag::SenderCompID>(header, senderCompId));
            ASSERT_EQ(senderCompId, "ABC");

            auto msgTypes = Fixpp::get<Fixpp::Tag::NoMsgTypes>(logon);
            ASSERT_EQ(msgTypes.size(), 2);

            std::string refMsgType;
            char msgDirection;

            ASSERT_TRUE(Fixpp::tryGet<Fixpp::Tag::RefMsgType>(msgTypes[0], refMsgType));
            ASSERT_EQ(refMsgType, "TEST");

            ASSERT_TRUE(Fixpp::tryGet<Fixpp::Tag::MsgDirection>(msgTypes[0], msgDirection));
            ASSERT_EQ(msgDirection, 'C');

            ASSERT_TRUE(Fixpp::tryGet<Fixpp::Tag::RefMsgType>(msgTypes[1], refMsgType));
            ASSERT_EQ(refMsgType, "TEST");
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

    using VisitRules = DefaultTestRules<Fixpp::v42::Spec::Dictionary>;

} // namespace should_try_get_fields_after_parsing

namespace should_be_able_to_return_value_in_visitor
{
    struct Visitor : public Fixpp::StaticVisitor<int>
    {
        int operator()(const Fixpp::v42::Header::Ref& header, const Fixpp::v42::Message::Logon::Ref& logon)
        {
            doAssert(header, logon);
            return 10;
        }

        void doAssert(const Fixpp::v42::Header::Ref& header, const Fixpp::v42::Message::Logon::Ref& logon)
        {
            std::string senderCompId;
            ASSERT_TRUE(Fixpp::tryGet<Fixpp::Tag::SenderCompID>(header, senderCompId));
            ASSERT_EQ(senderCompId, "ABC");

            auto msgTypes = Fixpp::get<Fixpp::Tag::NoMsgTypes>(logon);
            ASSERT_EQ(msgTypes.size(), 2);

            std::string refMsgType;
            char msgDirection;

            ASSERT_TRUE(Fixpp::tryGet<Fixpp::Tag::RefMsgType>(msgTypes[0], refMsgType));
            ASSERT_EQ(refMsgType, "TEST");

            ASSERT_TRUE(Fixpp::tryGet<Fixpp::Tag::MsgDirection>(msgTypes[0], msgDirection));
            ASSERT_EQ(msgDirection, 'C');

            ASSERT_TRUE(Fixpp::tryGet<Fixpp::Tag::RefMsgType>(msgTypes[1], refMsgType));
            ASSERT_EQ(refMsgType, "TEST");
        }

        template<typename HeaderT, typename MessageT> int operator()(HeaderT, MessageT)
        {
            return 0;
        }
    };

    using VisitRules = DefaultTestRules<Fixpp::v42::Spec::Dictionary>;

} // namespace should_be_able_to_return_value_in_visitor

namespace should_visit_repeating_group_in_logon_frame
{

    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v42::Header::Ref&, const Fixpp::v42::Message::Logon::Ref& logon)
        {
            auto msgTypes = Fixpp::get<Fixpp::Tag::NoMsgTypes>(logon);
            ASSERT_EQ(msgTypes.size(), 2);

            ASSERT_EQ(Fixpp::get<Fixpp::Tag::RefMsgType>(msgTypes[0]), "TEST");
            ASSERT_EQ(Fixpp::get<Fixpp::Tag::MsgDirection>(msgTypes[0]), 'C');

            ASSERT_EQ(Fixpp::get<Fixpp::Tag::RefMsgType>(msgTypes[1]), "TEST");
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

    using VisitRules = DefaultTestRules<Fixpp::v42::Spec::Dictionary>;

} // namespace should_visit_repeating_group_in_logon_frame

namespace should_visit_custom_message
{

    using MyTag = Fixpp::TagT<2154, Fixpp::Type::Int>;
    using MyMessage = Fixpp::ExtendedMessage<Fixpp::v42::Message::Logon, Fixpp::Required<MyTag>>;

    struct MyVisitRules : public Fixpp::VisitRules
    {
        using Overrides = OverrideSet<
            Override<Fixpp::v42::Message::Logon, As<MyMessage>>
        >;

        using Dictionary = Fixpp::v42::Spec::Dictionary;

        static constexpr bool ValidateChecksum = false;
        static constexpr bool ValidateLength = false;
        static constexpr bool StrictMode = false;
        static constexpr bool SkipUnknownTags = false;
    };

    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v42::Header::Ref& header, const MyMessage::Ref& message)
        {
            ASSERT_EQ(Fixpp::get<Fixpp::Tag::SenderCompID>(header), "ABC");
            ASSERT_EQ(Fixpp::get<Fixpp::Tag::HeartBtInt>(message), 60);

            ASSERT_EQ(Fixpp::get<MyTag>(message), 1212);
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

};

namespace should_visit_incremental_refresh_frame
{
    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v42::Header::Ref&, const Fixpp::v42::Message::MarketDataIncrementalRefresh::Ref& message)
        {
            using namespace Fixpp;

            ASSERT_EQ(Fixpp::get<Tag::MDReqID>(message), "1364");

            auto mdEntries = Fixpp::get<Tag::NoMDEntries>(message);
            ASSERT_EQ(mdEntries.size(), 4);

            auto entry0 = mdEntries[0];
            ASSERT_EQ(Fixpp::get<Tag::MDUpdateAction>(entry0), '2');
            ASSERT_EQ(Fixpp::get<Tag::Symbol>(entry0), "CHF/JPY");
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry0), '0');

            auto entry1 = mdEntries[1];
            ASSERT_EQ(Fixpp::get<Tag::MDUpdateAction>(entry1), '0');
            ASSERT_EQ(Fixpp::get<Tag::Symbol>(entry1), "CHF/JPY");
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry1), '0');

            auto entry2 = mdEntries[2];
            ASSERT_EQ(Fixpp::get<Tag::MDUpdateAction>(entry2), '2');
            ASSERT_EQ(Fixpp::get<Tag::Symbol>(entry2), "CHF/JPY");
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry2), '1');

            auto entry3 = mdEntries[3];
            ASSERT_EQ(Fixpp::get<Tag::MDUpdateAction>(entry3), '0');
            ASSERT_EQ(Fixpp::get<Tag::Symbol>(entry3), "CHF/JPY");
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry3), '1');
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

    using VisitRules = DefaultTestRules<Fixpp::v42::Spec::Dictionary>;
};

namespace should_visit_custom_snapshot_frame
{
    using MyTag = Fixpp::TagT<9063, Fixpp::Type::String>;

    struct SnapshotMessageOverwrite : public Fixpp::MessageOverwrite<Fixpp::v44::Message::MarketDataSnapshot>
    {
        using Changes = ChangeSet<
            ExtendGroup<Fixpp::Tag::NoMDEntries, MyTag>
        >;
    };

    using Snapshot = SnapshotMessageOverwrite::Changes::Apply;

    struct VisitRules : public Fixpp::VisitRules
    {
        using Overrides = OverrideSet<
            Override<Fixpp::v44::Message::MarketDataSnapshot, As<Snapshot>>
        >;

        using Dictionary = Fixpp::v44::Spec::Dictionary;

        static constexpr bool ValidateChecksum = false;
        static constexpr bool ValidateLength = false;
        static constexpr bool StrictMode = false;
        static constexpr bool SkipUnknownTags = false;
    };

    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v44::Header::Ref&, const Snapshot::Ref& message)
        {
            using namespace Fixpp;

            ASSERT_EQ(Fixpp::get<Tag::Symbol>(message), "AUD/CAD");
            ASSERT_EQ(Fixpp::get<Tag::MDReqID>(message), "1709");

            auto mdEntries = Fixpp::get<Tag::NoMDEntries>(message);
            ASSERT_EQ(mdEntries.size(), 2);

            auto entry0 = mdEntries[0];
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry0), '0');
            ASSERT_EQ(Fixpp::get<Tag::MDEntrySize>(entry0), 500000);
            ASSERT_EQ(Fixpp::get<Tag::QuoteEntryID>(entry0), "02z00000hdi:A");
            ASSERT_EQ(Fixpp::get<MyTag>(entry0), "MP");

            auto entry1 = mdEntries[1];
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry1), '1');
            ASSERT_EQ(Fixpp::get<Tag::MDEntrySize>(entry1), 500000);
            ASSERT_EQ(Fixpp::get<Tag::QuoteEntryID>(entry1), "02z00000hdi:A");
            ASSERT_EQ(Fixpp::get<MyTag>(entry1), "TP");
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };
}

namespace should_visit_nested_repeating_groups
{
    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v44::Header::Ref&, const Fixpp::v44::Message::MarketDataSnapshot::Ref& message)
        {
            using namespace Fixpp;

            auto underlyings = Fixpp::get<Tag::NoUnderlyings>(message);
            ASSERT_EQ(underlyings.size(), 1);

            auto underlying0 = underlyings[0];
            auto underlyingSecurityAltIDs = Fixpp::get<Tag::NoUnderlyingSecurityAltID>(underlying0);
            ASSERT_EQ(underlyingSecurityAltIDs.size(), 1);

            ASSERT_EQ(Fixpp::get<Tag::UnderlyingSecurityAltID>(underlyingSecurityAltIDs[0]), "TESTID");
            ASSERT_EQ(Fixpp::get<Tag::UnderlyingSecurityAltIDSource>(underlyingSecurityAltIDs[0]), "TESTSOURCE");

            ASSERT_EQ(Fixpp::get<Tag::UnderlyingProduct>(underlying0), 1);

            auto mdEntries = Fixpp::get<Tag::NoMDEntries>(message);
            ASSERT_EQ(mdEntries.size(), 2);

            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(mdEntries[0]), '0');
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(mdEntries[1]), '1');
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

    using VisitRules = DefaultTestRules<Fixpp::v44::Spec::Dictionary>;
}

namespace should_visit_empty_repeating_group
{
    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v44::Header::Ref&, const Fixpp::v44::Message::MarketDataSnapshot::Ref& message)
        {
            using namespace Fixpp;

            auto mdEntries = Fixpp::get<Tag::NoMDEntries>(message);
            ASSERT_EQ(mdEntries.size(), 0);
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

    using VisitRules = DefaultTestRules<Fixpp::v44::Spec::Dictionary>;
}

namespace should_visit_unknown_tags_in_non_strict_mode
{
    struct VisitRules : public Fixpp::VisitRules
    {
        using Overrides = OverrideSet<>;
        using Dictionary = Fixpp::v44::Spec::Dictionary;

        static constexpr bool ValidateChecksum = false;
        static constexpr bool ValidateLength = false;
        static constexpr bool StrictMode = false;
        static constexpr bool SkipUnknownTags = false;
    };

    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v44::Header::Ref&, const Fixpp::v44::Message::MarketDataSnapshot::Ref& message)
        {
            using namespace Fixpp;
            ASSERT_EQ(message.unparsed.size(), 1);
            checkUnparsed(message, 10721, "CUSTOM1");

            auto underlyings = Fixpp::get<Tag::NoUnderlyings>(message);
            auto underlying0 = underlyings[0];

            ASSERT_EQ(underlying0.unparsed.size(), 1);
            checkUnparsed(underlying0, 10541, "CUSTOM2");

            auto underlyingSecurityAltIDs = Fixpp::get<Tag::NoUnderlyingSecurityAltID>(underlying0);
            auto underlyingSecurityAltID0 = underlyingSecurityAltIDs[0];
            ASSERT_EQ(underlyingSecurityAltID0.unparsed.size(), 2);
            checkUnparsed(underlyingSecurityAltID0, 10872, "CUSTOM3");
            checkUnparsed(underlyingSecurityAltID0, 10873, "CUSTOM4");

            auto mdEntries = Fixpp::get<Tag::NoMDEntries>(message);
            ASSERT_EQ(mdEntries[0].unparsed.size(), 1);
            checkUnparsed(mdEntries[0], 10331, "CUSTOM5");

            ASSERT_EQ(mdEntries[1].unparsed.size(), 1);
            checkUnparsed(mdEntries[1], 10331, "CUSTOM6");
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }

        template<typename Message>
        void checkUnparsed(const Message& message, int tag, const char* value)
        {
            auto it = std::find_if(
                std::begin(message.unparsed), std::end(message.unparsed),
                [=](const typename Message::Unparsed& unparsed) { return unparsed.tag == tag; }
            );

            ASSERT_NE(it, message.unparsed.end());

            auto view = it->view;
            std::string str(view.first, view.second);
            ASSERT_EQ(str, value);
        }
    };
};

namespace should_convert_from_ref
{
    struct Visitor : public Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v42::Header::Ref& headerRef, const Fixpp::v42::Message::MarketDataIncrementalRefresh::Ref& message)
        {
            using namespace Fixpp;

            auto header = fromRef(headerRef);
            ASSERT_EQ(Fixpp::get<Tag::TargetCompID>(header), "MDABC");

            auto refresh = fromRef(message);

            ASSERT_EQ(Fixpp::get<Tag::MDReqID>(refresh), "1364");

            auto mdEntries = Fixpp::get<Tag::NoMDEntries>(refresh);
            ASSERT_EQ(mdEntries.size(), 4);

            auto entry0 = mdEntries[0];
            ASSERT_EQ(Fixpp::get<Tag::MDUpdateAction>(entry0), '2');
            ASSERT_EQ(Fixpp::get<Tag::Symbol>(entry0), "CHF/JPY");
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry0), '0');

            auto entry1 = mdEntries[1];
            ASSERT_EQ(Fixpp::get<Tag::MDUpdateAction>(entry1), '0');
            ASSERT_EQ(Fixpp::get<Tag::Symbol>(entry1), "CHF/JPY");
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry1), '0');

            auto entry2 = mdEntries[2];
            ASSERT_EQ(Fixpp::get<Tag::MDUpdateAction>(entry2), '2');
            ASSERT_EQ(Fixpp::get<Tag::Symbol>(entry2), "CHF/JPY");
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry2), '1');

            auto entry3 = mdEntries[3];
            ASSERT_EQ(Fixpp::get<Tag::MDUpdateAction>(entry3), '0');
            ASSERT_EQ(Fixpp::get<Tag::Symbol>(entry3), "CHF/JPY");
            ASSERT_EQ(Fixpp::get<Tag::MDEntryType>(entry3), '1');
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

    using VisitRules = DefaultTestRules<Fixpp::v42::Spec::Dictionary>;
};

namespace should_skip_unknown_tags
{
    using Message = Fixpp::VersionnedMessage<
        Fixpp::v44::Version,
        Fixpp::MessageType::MarketDataSnapshotFullRefresh,

        Fixpp::Tag::TransactTime,
        Fixpp::Tag::MDReqID,
        Fixpp::Tag::Symbol,
        Fixpp::RepeatingGroup<
            Fixpp::Tag::NoMDEntries,
            Fixpp::Tag::MDEntryType,
            Fixpp::Tag::MDEntryPx,
            Fixpp::Tag::MDEntrySize,
            Fixpp::Tag::NumberOfOrders,
            Fixpp::Tag::MDEntryDate,
            Fixpp::Tag::QuoteEntryID
        >
    >;

    namespace Spec
    {

        struct Dictionary
        {
            using Version = Fixpp::v44::Version;
            using Header = Fixpp::v44::Header;

            using Messages = typename meta::typelist::make<Message>::Result;

        };

    }

    struct VisitRules : public Fixpp::VisitRules
    {
        using Overrides = OverrideSet<>;
        using Dictionary = Spec::Dictionary;

        static constexpr bool ValidateChecksum = false;
        static constexpr bool ValidateLength = false;
        static constexpr bool StrictMode = false;
        static constexpr bool SkipUnknownTags = true;
    };

    struct Visitor : Fixpp::StaticVisitor<void>
    {
        void operator()(const Fixpp::v44::Header::Ref&, const Message::Ref& message)
        {
            ASSERT_TRUE(message.unparsed.empty());

            const auto& mdEntries = Fixpp::get<Fixpp::Tag::NoMDEntries>(message);
            ASSERT_EQ(mdEntries.size(), 2);

            auto mdEntry0 = mdEntries[0];
            ASSERT_TRUE(mdEntry0.unparsed.empty());

            auto mdEntry1 = mdEntries[1];
            ASSERT_TRUE(mdEntry1.unparsed.empty());
        }
    };
}

struct AssertVisitRules : public Fixpp::VisitRules
{
    using Overrides = OverrideSet<>;
    using Dictionary = Fixpp::v42::Spec::Dictionary;

    static constexpr bool ValidateChecksum = false;
    static constexpr bool ValidateLength = false;
    static constexpr bool StrictMode = true;
    static constexpr bool SkipUnknownTags = false;
};

struct AssertVisitor : public Fixpp::StaticVisitor<void>
{
    template<typename Header, typename Message>
    void operator()(Header, Message)
    {
        ASSERT_TRUE(false);
    }
};

template<typename Visitor, typename Rules>
Fixpp::VisitError<typename Visitor::ResultType> doVisit(const char* frame, Visitor visitor)
{
    return Fixpp::visit(frame, std::strlen(frame), visitor);
}

template<typename Visitor, typename Rules>
Fixpp::VisitError<typename Visitor::ResultType> doVisit(const char* frame, Visitor visitor, Rules rules)
{
    return Fixpp::visit(frame, std::strlen(frame), visitor, rules);
}

TEST(visitor_test, should_parse_utc_date)
{
    const char* str = "20171105";
    auto time = Fixpp::details::LexicalCast<Fixpp::Type::UTCDate>::cast(str, std::strlen(str));

    auto t = time.time();
    std::tm tm{};
    gmtime_r(&t, &tm);

    ASSERT_EQ(tm.tm_year, 117);
    ASSERT_EQ(tm.tm_mon, 10);
    ASSERT_EQ(tm.tm_mday, 5);
    ASSERT_EQ(tm.tm_hour, 0);
    ASSERT_EQ(tm.tm_min, 0);
    ASSERT_EQ(tm.tm_sec, 0);
}

TEST(visitor_test, should_parse_utc_time)
{
    const char* str = "14:09:30";
    auto time = Fixpp::details::LexicalCast<Fixpp::Type::UTCTimeOnly>::cast(str, std::strlen(str));

    auto t = time.time();
    std::tm tm{};
    gmtime_r(&t, &tm);

    ASSERT_EQ(tm.tm_hour, 14);
    ASSERT_EQ(tm.tm_min, 9);
    ASSERT_EQ(tm.tm_sec, 30);

    ASSERT_EQ(time.msec().has_value(), false);
    ASSERT_EQ(time.usec().has_value(), false);
}

TEST(visitor_test, should_parse_utc_time_with_milliseconds)
{
    const char* str = "14:09:30.125";
    auto time = Fixpp::details::LexicalCast<Fixpp::Type::UTCTimeOnly>::cast(str, std::strlen(str));

    auto t = time.time();
    std::tm tm{};
    gmtime_r(&t, &tm);

    ASSERT_EQ(tm.tm_hour, 14);
    ASSERT_EQ(tm.tm_min, 9);
    ASSERT_EQ(tm.tm_sec, 30);

    ASSERT_EQ(time.msec().value(), 125);
    ASSERT_EQ(time.usec().has_value(), false);
}

TEST(visitor_test, should_parse_utc_time_with_microseconds)
{
    const char* str = "14:09:30.125456";
    auto time = Fixpp::details::LexicalCast<Fixpp::Type::UTCTimeOnly>::cast(str, std::strlen(str));

    auto t = time.time();
    std::tm tm{};
    gmtime_r(&t, &tm);

    ASSERT_EQ(tm.tm_hour, 14);
    ASSERT_EQ(tm.tm_min, 9);
    ASSERT_EQ(tm.tm_sec, 30);

    ASSERT_EQ(time.msec().value(), 125);
    ASSERT_EQ(time.usec().value(), 456);
}

TEST(visitor_test, should_parse_utc_timestamp)
{
    const char* str = "20171105-14:09:30";
    auto time = Fixpp::details::LexicalCast<Fixpp::Type::UTCTimestamp>::cast(str, std::strlen(str));

    auto t = time.time();
    std::tm tm{};
    gmtime_r(&t, &tm);

    ASSERT_EQ(tm.tm_year, 117);
    ASSERT_EQ(tm.tm_mon, 10);
    ASSERT_EQ(tm.tm_mday, 5);
    ASSERT_EQ(tm.tm_hour, 14);
    ASSERT_EQ(tm.tm_min, 9);
    ASSERT_EQ(tm.tm_sec, 30);

    ASSERT_EQ(time.msec().has_value(), false);
    ASSERT_EQ(time.usec().has_value(), false);
}

TEST(visitor_test, should_parse_utc_timestamp_with_milliseconds)
{
    const char* str = "20171105-14:09:30.125";
    auto time = Fixpp::details::LexicalCast<Fixpp::Type::UTCTimestamp>::cast(str, std::strlen(str));

    auto t = time.time();
    std::tm tm{};
    gmtime_r(&t, &tm);

    ASSERT_EQ(tm.tm_year, 117);
    ASSERT_EQ(tm.tm_mon, 10);
    ASSERT_EQ(tm.tm_mday, 5);
    ASSERT_EQ(tm.tm_hour, 14);
    ASSERT_EQ(tm.tm_min, 9);
    ASSERT_EQ(tm.tm_sec, 30);

    ASSERT_EQ(time.msec().value(), 125);
    ASSERT_EQ(time.usec().has_value(), false);
}

TEST(visitor_test, should_parse_utc_timestamp_with_microseconds)
{
    const char* str = "20171105-14:09:30.125456";
    auto time = Fixpp::details::LexicalCast<Fixpp::Type::UTCTimestamp>::cast(str, std::strlen(str));

    auto t = time.time();
    std::tm tm{};
    gmtime_r(&t, &tm);

    ASSERT_EQ(tm.tm_year, 117);
    ASSERT_EQ(tm.tm_mon, 10);
    ASSERT_EQ(tm.tm_mday, 5);
    ASSERT_EQ(tm.tm_hour, 14);
    ASSERT_EQ(tm.tm_min, 9);
    ASSERT_EQ(tm.tm_sec, 30);

    ASSERT_EQ(time.msec().value(), 125);
    ASSERT_EQ(time.usec().value(), 456);
}

TEST(visitor_test, should_visit_logon_frame)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|56=TT_ORDER|96=12345678|98=0|108=60|141=Y|10=248";
    auto err = doVisit(frame, should_visit_logon_frame::Visitor(), should_visit_logon_frame::VisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_visit_repeating_group_in_logon_frame)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|56=TT_ORDER|96=12345678|384=2|372=TEST|385=C|372=TEST|10=248";
    auto err = doVisit(frame, should_visit_repeating_group_in_logon_frame::Visitor(), should_visit_repeating_group_in_logon_frame::VisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_visit_custom_message)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|56=TT_ORDER|96=12345678|2154=1212|98=0|108=60|141=Y|10=248";
    auto err = doVisit(frame, should_visit_custom_message::Visitor(), should_visit_custom_message::MyVisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_visit_incremental_refresh_frame)
{
    const char* frame = "8=FIX.4.2|9=407|35=X|34=002565204|52=20160908-08:42:10.359|49=Prov|56=MDABC|262=1364|268=4|"
                        "279=2|55=CHF/JPY|269=0|278=0453665272|270=00104.840000|271=001000000.00|15=CHF|"
                        "279=0|55=CHF/JPY|269=0|278=0453665276|270=00104.841000|271=001000000.00|15=CHF|"
                        "279=2|55=CHF/JPY|269=1|278=0453665273|270=00104.855000|271=001000000.00|15=CHF|"
                        "279=0|55=CHF/JPY|269=1|278=0453665277|270=00104.856000|271=001000000.00|15=CHF|"
                        "10=213";

    auto err = doVisit(frame, should_visit_incremental_refresh_frame::Visitor(), should_visit_incremental_refresh_frame::VisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_visit_snapshot_frame)
{
    const char* frame = "8=FIX.4.4|9=0230|35=W|49=Prov|56=MDABC|34=2289004|52=20161229-16:18:09.098|55=AUD/CAD|262=1709|268=2|"
                        "269=0|270=0.97285|271=500000|272=20170103|299=02z00000hdi:A|9063=MP|"
                        "269=1|270=0.97309|271=500000|272=20170103|299=02z00000hdi:A|9063=TP|"
                        "10=233|";

    using Visitor = should_visit_custom_snapshot_frame::Visitor;
    using VisitRules = should_visit_custom_snapshot_frame::VisitRules;

   auto err = doVisit(frame, Visitor(), VisitRules());
   ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_visit_nested_repeating_groups)
{
    const char* frame = "8=FIX.4.4|9=0000|35=W|49=Prov|56=MDABC|55=AUD/CAD|262=1709|"
                        "711=1|"
                            "311=AUD/CAD|"
                            "457=1|"
                                "458=TESTID|459=TESTSOURCE|"
                            "462=1|"
                        "292=D|268=2|"
                            "269=0|271=500000|272=20170103|299=02z00000hdi:A|"
                            "269=1|271=500000|272=20170103|299=02z00000hdi:A|"
                        "10=213";
    auto err = doVisit(frame, should_visit_nested_repeating_groups::Visitor(), should_visit_nested_repeating_groups::VisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_visit_empty_repeating_group)
{
    const char* frame = "8=FIX.4.4|9=0000|35=W|49=Prov|56=MDABC|55=AUD/CAD|262=1709|268=0|10=213";
    auto err = doVisit(frame, should_visit_empty_repeating_group::Visitor(), should_visit_empty_repeating_group::VisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_visit_unknown_tags_in_non_strict_mode)
{
    const char* frame = "8=FIX.4.4|9=0000|35=W|49=Prov|56=MDABC|55=AUD/CAD|262=1709|"
                        "10721=CUSTOM1|"
                        "711=1|"
                            "311=AUD/CAD|"
                            "10541=CUSTOM2|"
                            "457=1|"
                                "458=TESTID|459=TESTSOURCE|10872=CUSTOM3|10873=CUSTOM4|"
                            "462=1|"
                        "292=D|268=2|"
                            "269=0|271=500000|272=20170103|299=02z00000hdi:A|10331=CUSTOM5|"
                            "269=1|271=500000|272=20170103|299=02z00000hdi:A|10331=CUSTOM6|"
                        "10=213";
    using Visitor = should_visit_unknown_tags_in_non_strict_mode::Visitor;
    using VisitRules = should_visit_unknown_tags_in_non_strict_mode::VisitRules;

    auto err = doVisit(frame, Visitor(), VisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_stop_when_encountering_invalid_fix_version)
{
    const char* frame = "8=FIX.5.1|9=0000|35=0|49=Prov|56=MDABC|10=213";

    auto error = doVisit(frame, AssertVisitor(), AssertVisitRules());
    ASSERT_FALSE(error.isOk());

    auto errorKind = error.unwrapErr();
    ASSERT_EQ(errorKind.type(), Fixpp::ErrorKind::InvalidVersion);
}

TEST(visitor_test, should_stop_in_strict_mode_when_encountering_an_unknown_tag)
{
    const char* frame = "8=FIX.4.2|9=0000|35=0|49=Prov|56=MDABC|221=A|10=213";

    auto error = doVisit(frame, AssertVisitor(), AssertVisitRules());
    ASSERT_FALSE(error.isOk());

    auto errorKind = error.unwrapErr();
    ASSERT_EQ(errorKind.type(), Fixpp::ErrorKind::UnknownTag);
    ASSERT_EQ(errorKind.asString(), "Encountered unknown tag 221");
}

TEST(visitor_test, should_try_get_fields_after_parsing)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|56=TT_ORDER|96=12345678|384=2|372=TEST|385=C|372=TEST|10=248";

    auto err = doVisit(frame, should_try_get_fields_after_parsing::Visitor(), should_try_get_fields_after_parsing::VisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_be_able_to_return_value_in_visitor)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|56=TT_ORDER|96=12345678|384=2|372=TEST|385=C|372=TEST|10=248";

    auto err = doVisit(frame, should_be_able_to_return_value_in_visitor::Visitor(), should_be_able_to_return_value_in_visitor::VisitRules());
    ASSERT_EQ(err.unwrapOr(0), 10);
}

TEST(visitor_test, should_visit_tag)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|56=TT_ORDER|96=12345678|384=2|372=TEST|385=C|372=TEST|10=248|";

    auto assertFalse = [&](const Fixpp::ErrorKind&) { ASSERT_TRUE(false); };

    Fixpp::visitTag<Fixpp::Tag::MsgType>(frame, std::strlen(frame))
      .then([&](const std::string& msgType) { ASSERT_EQ(msgType, "A"); })
      .otherwise(assertFalse);

    Fixpp::visitTag<Fixpp::Tag::RefMsgType>(frame, std::strlen(frame))
        .then([&](const std::string& ref) { ASSERT_EQ(ref, "TEST"); })
        .otherwise(assertFalse);

    Fixpp::visitTag<Fixpp::Tag::BodyLength>(frame, std::strlen(frame))
        .then([&](int64_t length) { ASSERT_EQ(length, 84); })
        .otherwise(assertFalse);

    Fixpp::visitTag<Fixpp::Tag::OnBehalfOfCompID>(frame, std::strlen(frame))
        .then([&](const std::string&) { ASSERT_TRUE(false); })
        .otherwise([&](const Fixpp::ErrorKind& e) { ASSERT_EQ(e.type(), Fixpp::ErrorKind::UnknownTag); });
}

TEST(visitor_test, should_convert_from_ref)
{
    const char* frame = "8=FIX.4.2|9=407|35=X|34=002565204|52=20160908-08:42:10.359|49=Prov|56=MDABC|262=1364|268=4|"
                        "279=2|55=CHF/JPY|269=0|278=0453665272|270=00104.840000|271=001000000.00|15=CHF|"
                        "279=0|55=CHF/JPY|269=0|278=0453665276|270=00104.841000|271=001000000.00|15=CHF|"
                        "279=2|55=CHF/JPY|269=1|278=0453665273|270=00104.855000|271=001000000.00|15=CHF|"
                        "279=0|55=CHF/JPY|269=1|278=0453665277|270=00104.856000|271=001000000.00|15=CHF|"
                        "10=213";

    auto err = doVisit(frame, should_convert_from_ref::Visitor(), should_convert_from_ref::VisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_visit_message_with_multiple_chars_message_type)
{
    const char* frame = "8=FIX.4.4|9=84|35=BF|923=123|924=1|553=username|10=248";

    auto err = doVisit(frame, should_visit_message_with_multiple_chars_message_type::Visitor(), should_visit_message_with_multiple_chars_message_type::VisitRules());
    ASSERT_TRUE(err.isOk());
}

TEST(visitor_test, should_skip_unknown_tags)
{
    const char* frame = "8=FIX.4.4|9=0234|35=W|49=SNDR|59=ABC|34=1|52=20170609-09:27:24|55=AAA/BBB|262=123|268=2|"
                        "269=0|270=1.181|271=50000|272=20170613|299=ABCDEFGH|9063=AB|"
                        "269=1|270=1.1182|271=50000|272=20170613|299=ABCDEFH|9063=AB|"
                        "9066=189718761|10=076|";

    auto err = doVisit(frame, should_skip_unknown_tags::Visitor(), should_skip_unknown_tags::VisitRules());
    ASSERT_TRUE(err.isOk());
}
