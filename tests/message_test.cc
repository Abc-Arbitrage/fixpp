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
