#include "gtest/gtest.h"
#include <fixpp/versions/v42.h>

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

    auto group = Fix::createGroup<Tag::NoMsgTypes>(logon, 2);
    auto instance = group.instance();

    Fix::set<Tag::RefMsgType>(instance, "TEST");
    Fix::set<Tag::MsgDirection>(instance, 'S');

    group.add(instance);

    Fix::set<Tag::RefMsgType>(instance, "MD");
    Fix::set<Tag::MsgDirection>(instance, 'S');

    group.add(instance);

    ASSERT_EQ(group.size(), 2);
}
