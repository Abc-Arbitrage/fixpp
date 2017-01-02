#include "gtest/gtest.h"

#include <cstring>

#include <fixpp/versions/v42.h>

TEST(message_test, should_bring_correct_version_with_message_type)
{
    constexpr const char* v1 = Fix::v42::Message::Heartbeat::Version::Str;
    constexpr const char* v2 = Fix::v42::Message::Logon::Version::Str;

    ASSERT_EQ(v1, "FIX.4.2");
    ASSERT_EQ(v2, "FIX.4.2");
}

TEST(message_test, should_get_and_set_tags)
{
    Fix::v42::Message::Logon logon;

    using namespace Fix;

    Fix::set<Tag::EncryptMethod>(logon, 1);
    ASSERT_EQ(Fix::get<Tag::EncryptMethod>(logon), 1);

    Fix::set<Tag::HeartBtInt>(logon, 30);
    ASSERT_EQ(Fix::get<Tag::HeartBtInt>(logon), 30);
}

TEST(message_test, should_create_repeating_group)
{
    Fix::v42::Message::Logon logon;

    using namespace Fix;

    // First, we create a new group and add 2 instances

    auto group = Fix::createGroup<Tag::NoMsgTypes>(logon, 2);
    auto instance = group.instance();

    Fix::set<Tag::RefMsgType>(instance, "TEST");
    Fix::set<Tag::MsgDirection>(instance, 'S');

    ASSERT_EQ(Fix::get<Tag::RefMsgType>(instance), "TEST");
    ASSERT_EQ(Fix::get<Tag::MsgDirection>(instance), 'S');

    group.add(instance);

    Fix::set<Tag::RefMsgType>(instance, "MD");
    Fix::set<Tag::MsgDirection>(instance, 'S');

    ASSERT_EQ(Fix::get<Tag::RefMsgType>(instance), "MD");
    ASSERT_EQ(Fix::get<Tag::MsgDirection>(instance), 'S');

    group.add(instance);

    ASSERT_EQ(group.size(), 2);

    // Let's now check that values have been written in the message itself
    auto& values = logon.values;

    // NoMsgTypes RepeatinGroup if the 6th field in the Logon message
    auto& repeatingGroup = std::get<6>(values);

    ASSERT_FALSE(repeatingGroup.empty());
    ASSERT_EQ(repeatingGroup.size(), 2);

    const auto& groupValues = repeatingGroup.get();
    ASSERT_EQ(groupValues.size(), 2);

    ASSERT_EQ(std::get<0>(groupValues[0]).get(), "TEST");
    ASSERT_EQ(std::get<1>(groupValues[0]).get(), 'S');

    ASSERT_EQ(std::get<0>(groupValues[1]).get(), "MD");
    ASSERT_EQ(std::get<1>(groupValues[1]).get(), 'S');
}

TEST(message_test, should_extend_message_properly)
{
    using MyTag = Fix::TagT<2154, Fix::Type::Int, Fix::Optional>;
    using MyMessage = Fix::ExtendedMessage<Fix::v42::Message::Logon, MyTag>;

    static constexpr size_t LogonTags = Fix::v42::Message::Logon::TotalTags;
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
        meta::typelist::ops::First<Fix::v42::Message::Logon::List>::Result;
    using FirstMyMessageTag =
        meta::typelist::ops::First<MyMessage::List>::Result;

    static constexpr size_t FirstLogonTagId = FirstLogonTag::Id;
    static constexpr size_t FirstMyMessageTagId = FirstMyMessageTag::Id;
    ASSERT_EQ(FirstLogonTagId, FirstMyMessageTagId);
}

TEST(message_test, should_optionally_get)
{
    using namespace Fix;

    Fix::v42::Message::IndicationOfInterest ioi;

    Fix::set<Tag::Symbol>(ioi, "MySymbol");
    Fix::set<Tag::Price>(ioi, 10.5);

    std::string symbol;
    float price;

    ASSERT_TRUE(Fix::tryGet<Tag::Symbol>(ioi, symbol));
    ASSERT_EQ(symbol, "MySymbol");

    ASSERT_TRUE(Fix::tryGet<Tag::Price>(ioi, price));
    ASSERT_EQ(price, 10.5);

    std::string currency;

    ASSERT_FALSE(Fix::tryGet<Tag::Currency>(ioi, currency));
    ASSERT_EQ(currency, std::string());
}
