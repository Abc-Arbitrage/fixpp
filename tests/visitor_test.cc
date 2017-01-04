#include "gtest/gtest.h"

#include <fixpp/versions/v42.h>
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
