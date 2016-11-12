#include "gtest/gtest.h"
#include <fixpp/versions/v42.h>
#include <fixpp/writer.h>

TEST(message_test, get_and_set)
{
    Fix::v42::Message::Logon logon;

    using namespace Fix;

    Fix::set<Tag::EncryptMethod>(logon, 1);
    ASSERT_EQ(Fix::get<Tag::EncryptMethod>(logon), 1);

    Fix::set<Tag::HeartBtInt>(logon, 30);
    ASSERT_EQ(Fix::get<Tag::HeartBtInt>(logon), 30);
}

TEST(message_test, group_tests)
{
    Fix::v42::Message::Logon logon;

    using namespace Fix;

    // First, we create a new group add 2 instances

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
