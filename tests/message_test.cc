#include "gtest/gtest.h"

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v44.h>

TEST(message_test, should_bring_correct_version_with_message_type)
{
    constexpr const char* v1 = Fixpp::v42::Message::Heartbeat::Version::Str;
    constexpr const char* v2 = Fixpp::v42::Message::Logon::Version::Str;

    ASSERT_EQ(std::string(v1), std::string("FIX.4.2"));
    ASSERT_EQ(std::string(v2), std::string("FIX.4.2"));
}

TEST(message_test, should_compare_version)
{
    using v42 = Fixpp::v42::Version;
    ASSERT_TRUE(v42::equals("FIX.4.2", 7));
}

TEST(message_test, should_get_and_set_tags)
{
    Fixpp::v42::Message::Logon logon;

    using namespace Fixpp;

    Fixpp::set<Tag::EncryptMethod>(logon, 1);
    ASSERT_EQ(Fixpp::get<Tag::EncryptMethod>(logon), 1);

    Fixpp::set<Tag::HeartBtInt>(logon, 30);
    ASSERT_EQ(Fixpp::get<Tag::HeartBtInt>(logon), 30);
}

TEST(message_test, should_create_repeating_group)
{
    Fixpp::v42::Message::Logon logon;

    using namespace Fixpp;

    // First, we create a new group and add 2 instances

    auto group = Fixpp::createGroup<Tag::NoMsgTypes>(logon, 2);
    auto instance = group.instance();

    Fixpp::set<Tag::RefMsgType>(instance, "TEST");
    Fixpp::set<Tag::MsgDirection>(instance, 'S');

    ASSERT_EQ(Fixpp::get<Tag::RefMsgType>(instance), "TEST");
    ASSERT_EQ(Fixpp::get<Tag::MsgDirection>(instance), 'S');

    group.add(instance);

    Fixpp::set<Tag::RefMsgType>(instance, "MD");
    Fixpp::set<Tag::MsgDirection>(instance, 'S');

    ASSERT_EQ(Fixpp::get<Tag::RefMsgType>(instance), "MD");
    ASSERT_EQ(Fixpp::get<Tag::MsgDirection>(instance), 'S');

    group.add(instance);

    ASSERT_EQ(group.size(), 2);

    // Let's now check that values have been written in the message itself
    auto& values = logon.values;

    // NoMsgTypes RepeatinGroup is the 7th field in the Logon message
    auto& repeatingGroup = meta::get<6>(values);

    ASSERT_FALSE(repeatingGroup.empty());
    ASSERT_EQ(repeatingGroup.size(), 2);

    const auto& groupValues = repeatingGroup.get();
    ASSERT_EQ(groupValues.size(), 2);

    ASSERT_EQ(meta::get<0>(groupValues[0].values).get(), "TEST");
    ASSERT_EQ(meta::get<1>(groupValues[0].values).get(), 'S');

    ASSERT_EQ(meta::get<0>(groupValues[1].values).get(), "MD");
    ASSERT_EQ(meta::get<1>(groupValues[1].values).get(), 'S');
}

TEST(message_test, should_throw_when_missing_fields_in_repeating_group)
{
    using namespace Fixpp;

    Fixpp::v42::Message::MarketDataIncrementalRefresh refresh;
    auto group = Fixpp::createGroup<Tag::NoMDEntries>(refresh, 1);
    auto instance = group.instance();

    Fixpp::set<Tag::MDEntryID>(instance, "TEST");
    ASSERT_THROW(group.add(instance), std::runtime_error);

    auto instance2 = group.instance();
    Fixpp::set<Tag::MDUpdateAction>(instance2, '0');
    ASSERT_NO_THROW(group.add(instance2));
}

TEST(message_test, should_throw_when_getting_not_present_tag)
{
    Fixpp::v42::Message::Logon logon;

    Fixpp::set<Fixpp::Tag::EncryptMethod>(logon, 1);
    Fixpp::set<Fixpp::Tag::HeartBtInt>(logon, 30);

    ASSERT_THROW(Fixpp::get<Fixpp::Tag::ResetSeqNumFlag>(logon), std::runtime_error);
}

TEST(message_test, should_extend_message_properly)
{
    using MyTag = Fixpp::TagT<2154, Fixpp::Type::Int>;
    using MyMessage = Fixpp::ExtendedMessage<Fixpp::v42::Message::Logon, MyTag>;

    static constexpr size_t LogonTags = Fixpp::v42::Message::Logon::TotalTags;
    static constexpr size_t MyMessageTags = MyMessage::TotalTags;

    // MyMessage should have one more tag
    ASSERT_EQ(MyMessageTags, LogonTags + 1);

    // Last tag of MyMessage should by MyTag
    using LastMyMessageTag =
        meta::typelist::ops::Last<MyMessage::List>::Result;

    static constexpr size_t LastMyMessageTagId = LastMyMessageTag::Id;
    ASSERT_EQ(LastMyMessageTagId, 2154);

    // First tag of MymEssage should by first tag of Logon
    using FirstLogonTag =
        meta::typelist::ops::First<Fixpp::v42::Message::Logon::TagsList>::Result;
    using FirstMyMessageTag =
        meta::typelist::ops::First<MyMessage::TagsList>::Result;

    static constexpr size_t FirstLogonTagId = FirstLogonTag::Id;
    static constexpr size_t FirstMyMessageTagId = FirstMyMessageTag::Id;
    ASSERT_EQ(FirstLogonTagId, FirstMyMessageTagId);
}

TEST(message_test, should_optionally_get)
{
    using namespace Fixpp;

    Fixpp::v42::Message::IndicationOfInterest ioi;

    Fixpp::set<Tag::Symbol>(ioi, "MySymbol");
    Fixpp::set<Tag::Price>(ioi, 10.5);

    std::string symbol;
    double price;

    ASSERT_TRUE(Fixpp::tryGet<Tag::Symbol>(ioi, symbol));
    ASSERT_EQ(symbol, "MySymbol");

    ASSERT_TRUE(Fixpp::tryGet<Tag::Price>(ioi, price));
    ASSERT_EQ(price, 10.5);

    std::string currency;

    ASSERT_FALSE(Fixpp::tryGet<Tag::Currency>(ioi, currency));
    ASSERT_EQ(currency, std::string());
}

TEST(message_test, should_overwrite_tags_in_message)
{
    using namespace Fixpp;
    using MyTag = Fixpp::TagT<2154, Fixpp::Type::Int>;

    struct LogonOverwrite : public Fixpp::MessageOverwrite<Fixpp::v42::Message::Logon>
    {
        using OverwriteHeartBtInt = ChangeType<Of<Tag::HeartBtInt>, To<Type::String>>;
        using HeartBtStr = typename OverwriteHeartBtInt::Type;

        using Changes = ChangeSet<
           OverwriteHeartBtInt,
           AddTag<MyTag>
      >;
    };

    using MyMessage = LogonOverwrite::Changes::Apply;
    using HeartBtStr = LogonOverwrite::HeartBtStr;

    static constexpr size_t LogonTags = Fixpp::v42::Message::Logon::TotalTags;
    static constexpr size_t MyMessageTags = MyMessage::TotalTags;

    // MyMessage should have one more tag
    ASSERT_EQ(MyMessageTags, LogonTags + 1);

    MyMessage message;

    Fixpp::set<HeartBtStr>(message, "30s");
    ASSERT_EQ(Fixpp::get<HeartBtStr>(message), "30s");

    Fixpp::set<MyTag>(message, 1212);
    ASSERT_EQ(Fixpp::get<MyTag>(message), 1212);
}
