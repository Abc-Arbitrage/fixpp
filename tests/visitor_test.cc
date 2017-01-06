#include "gtest/gtest.h"

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v44.h>
#include <fixpp/visitor.h>

namespace should_visit_logon_frame
{
    struct Visitor
    {
        void operator()(const Fix::v42::Header::Ref& header, const Fix::v42::Message::Logon::Ref& logon)
        {
            ASSERT_EQ(Fix::get<Fix::Tag::SenderCompID>(header), "ABC");
            ASSERT_EQ(Fix::get<Fix::Tag::HeartBtInt>(logon), 60);
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

} // namespace should_visit_logon_frame

namespace should_visit_repeating_group_in_logon_frame
{

    struct Visitor
    {
        void operator()(const Fix::v42::Header::Ref& header, const Fix::v42::Message::Logon::Ref& logon)
        {
            auto msgTypes = Fix::get<Fix::Tag::NoMsgTypes>(logon);
            ASSERT_EQ(msgTypes.size(), 2);

            ASSERT_EQ(Fix::get<Fix::Tag::RefMsgType>(msgTypes[0]), "TEST");
            ASSERT_EQ(Fix::get<Fix::Tag::MsgDirection>(msgTypes[0]), 'C');

            ASSERT_EQ(Fix::get<Fix::Tag::RefMsgType>(msgTypes[1]), "TEST");
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };

} // namespace should_visit_repeating_group_in_logon_frame

namespace should_visit_custom_message
{

    using MyTag = Fix::TagT<2154, Fix::Type::Int>;
    using MyMessage = Fix::ExtendedMessage<Fix::v42::Message::Logon, Fix::Required<MyTag>>;

    struct MyVisitRules : public Fix::VisitRules
    {
        using Overrides = OverrideSet<
            Override<Fix::v42::Message::Logon, As<MyMessage>>
        >;

        static constexpr bool ValidateChecksum = false;
        static constexpr bool ValidateLength = false;
    };

    struct Visitor
    {
        void operator()(const Fix::v42::Header::Ref& header, const MyMessage::Ref& message)
        {
            ASSERT_EQ(Fix::get<Fix::Tag::SenderCompID>(header), "ABC");
            ASSERT_EQ(Fix::get<Fix::Tag::HeartBtInt>(message), 60);

            ASSERT_EQ(Fix::get<MyTag>(message), 1212);
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };
};

namespace should_visit_incremental_refresh_frame
{
    struct Visitor
    {
        void operator()(const Fix::v42::Header::Ref& header, const Fix::v42::Message::MarketDataIncrementalRefresh::Ref& message)
        {
            using namespace Fix;

            ASSERT_EQ(Fix::get<Tag::MDReqID>(message), "1364");

            auto mdEntries = Fix::get<Tag::NoMDEntries>(message);
            ASSERT_EQ(mdEntries.size(), 4);

            auto entry0 = mdEntries[0];
            ASSERT_EQ(Fix::get<Tag::MDUpdateAction>(entry0), '2');
            ASSERT_EQ(Fix::get<Tag::Symbol>(entry0), "CHF/JPY");
            ASSERT_EQ(Fix::get<Tag::MDEntryType>(entry0), '0');

            auto entry1 = mdEntries[1];
            ASSERT_EQ(Fix::get<Tag::MDUpdateAction>(entry1), '0');
            ASSERT_EQ(Fix::get<Tag::Symbol>(entry1), "CHF/JPY");
            ASSERT_EQ(Fix::get<Tag::MDEntryType>(entry1), '0');

            auto entry2 = mdEntries[2];
            ASSERT_EQ(Fix::get<Tag::MDUpdateAction>(entry2), '2');
            ASSERT_EQ(Fix::get<Tag::Symbol>(entry2), "CHF/JPY");
            ASSERT_EQ(Fix::get<Tag::MDEntryType>(entry2), '1');

            auto entry3 = mdEntries[3];
            ASSERT_EQ(Fix::get<Tag::MDUpdateAction>(entry3), '0');
            ASSERT_EQ(Fix::get<Tag::Symbol>(entry3), "CHF/JPY");
            ASSERT_EQ(Fix::get<Tag::MDEntryType>(entry3), '1');
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };
};

namespace should_visit_custom_snapshot_frame
{
    using MyTag = Fix::TagT<9063, Fix::Type::String>;

    struct SnapshotMessageOverwrite : public Fix::MessageOverwrite<Fix::v44::Message::MarketDataSnapshot>
    {
        using Changes = ChangeSet<
            ExtendGroup<Fix::Tag::NoMDEntries, MyTag>
        >;
    };

    using Snapshot = SnapshotMessageOverwrite::Changes::Apply;

    struct VisitRules : public Fix::VisitRules
    {
        using Overrides = OverrideSet<
            Override<Fix::v44::Message::MarketDataSnapshot, As<Snapshot>>
        >;

        static constexpr bool ValidateChecksum = false;
        static constexpr bool ValidateLength = false;
    };

    struct Visitor
    {
        void operator()(const Fix::v44::Header::Ref& header, const Snapshot::Ref& message)
        {
            using namespace Fix;

            ASSERT_EQ(Fix::get<Tag::Symbol>(message), "AUD/CAD");
            ASSERT_EQ(Fix::get<Tag::MDReqID>(message), "1709");

            auto mdEntries = Fix::get<Tag::NoMDEntries>(message);
            ASSERT_EQ(mdEntries.size(), 2);

            auto entry0 = mdEntries[0];
            ASSERT_EQ(Fix::get<Tag::MDEntryType>(entry0), '0');
            //ASSERT_EQ(Fix::get<Tag::MDEntrySize>(entry0), 500000);
            ASSERT_EQ(Fix::get<Tag::QuoteEntryID>(entry0), "02z00000hdi:A");
            ASSERT_EQ(Fix::get<MyTag>(entry0), "MP");

            auto entry1 = mdEntries[1];
            ASSERT_EQ(Fix::get<Tag::MDEntryType>(entry1), '1');
            //ASSERT_EQ(Fix::get<Tag::MDEntrySize>(entry1), 500000);
            ASSERT_EQ(Fix::get<Tag::QuoteEntryID>(entry1), "02z00000hdi:A");
            ASSERT_EQ(Fix::get<MyTag>(entry1), "TP");
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };
}

namespace should_visit_nested_repeating_groups
{
    struct Visitor
    {
        void operator()(const Fix::v44::Header::Ref& header, const Fix::v44::Message::MarketDataSnapshot::Ref& message)
        {
            using namespace Fix;

            auto underlyings = Fix::get<Tag::NoUnderlyings>(message);
            ASSERT_EQ(underlyings.size(), 1);

            auto underlying0 = underlyings[0];
            auto underlyingSecurityAltIDs = Fix::get<Tag::NoUnderlyingSecurityAltID>(underlying0);
            ASSERT_EQ(underlyingSecurityAltIDs.size(), 1);

            ASSERT_EQ(Fix::get<Tag::UnderlyingSecurityAltID>(underlyingSecurityAltIDs[0]), "TESTID");
            ASSERT_EQ(Fix::get<Tag::UnderlyingSecurityAltIDSource>(underlyingSecurityAltIDs[0]), "TESTSOURCE");

            ASSERT_EQ(Fix::get<Tag::UnderlyingProduct>(underlying0), 1);

            auto mdEntries = Fix::get<Tag::NoMDEntries>(message);
            ASSERT_EQ(mdEntries.size(), 2);

            ASSERT_EQ(Fix::get<Tag::MDEntryType>(mdEntries[0]), '0');
            ASSERT_EQ(Fix::get<Tag::MDEntryType>(mdEntries[1]), '1');
        }

        template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
        {
            ASSERT_TRUE(false);
        }
    };
}

template<typename Visitor>
void doVisit(const char* frame, Visitor visitor)
{
    Fix::visit(frame, std::strlen(frame), visitor);
}

TEST(visitor_test, should_visit_logon_frame)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|56=TT_ORDER|96=12345678|98=0|108=60|141=Y|10=248";
    doVisit(frame, should_visit_logon_frame::Visitor());
}

TEST(visitor_test, should_visit_repeating_group_in_logon_frame)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|56=TT_ORDER|96=12345678|384=2|372=TEST|385=C|372=TEST|10=248";
    doVisit(frame, should_visit_repeating_group_in_logon_frame::Visitor());
}

TEST(visitor_test, should_visit_custom_message)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|2154=1212|56=TT_ORDER|96=12345678|98=0|108=60|141=Y|10=248";
    Fix::visit(frame, std::strlen(frame), should_visit_custom_message::Visitor(), should_visit_custom_message::MyVisitRules());
}

TEST(visitor_test, should_visit_incremental_refresh_frame)
{
    const char* frame = "8=FIX.4.2|9=407|35=X|34=002565204|52=20160908-08:42:10.359|49=Prov|56=MDABC|262=1364|268=4|"
                        "279=2|55=CHF/JPY|269=0|278=0453665272|270=00104.840000|271=001000000.00|15=CHF|"
                        "279=0|55=CHF/JPY|269=0|278=0453665276|270=00104.841000|271=001000000.00|15=CHF|"
                        "279=2|55=CHF/JPY|269=1|278=0453665273|270=00104.855000|271=001000000.00|15=CHF|"
                        "279=0|55=CHF/JPY|269=1|278=0453665277|270=00104.856000|271=001000000.00|15=CHF|"
                        "10=213";

    doVisit(frame, should_visit_incremental_refresh_frame::Visitor());
}

TEST(visitor_test, should_visit_snapshot_frame)
{
    const char* frame = "8=FIX.4.4|9=0230|35=W|49=Prov|56=MDABC|34=2289004|52=20161229-16:18:09.098|55=AUD/CAD|262=1709|268=2|"
                        "269=0|270=0.97285|271=500000|272=20170103|299=02z00000hdi:A|9063=MP|"
                        "269=1|270=0.97309|271=500000|272=20170103|299=02z00000hdi:A|9063=TP|"
                        "10=233|";

    using Visitor = should_visit_custom_snapshot_frame::Visitor;
    using VisitRules = should_visit_custom_snapshot_frame::VisitRules;

    Fix::visit(frame, std::strlen(frame), Visitor(), VisitRules());
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
    doVisit(frame, should_visit_nested_repeating_groups::Visitor());
}
