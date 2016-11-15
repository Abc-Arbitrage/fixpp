#include "gtest/gtest.h"

#include <fixpp/message.h>
#include <fixpp/versions/v42.h>
#include <fixpp/visitor.h>

struct MyVisitor
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

TEST(visitor_test, should_visit_logon_frame)
{
    const char* frame = "8=FIX.4.2|9=84|35=A|34=1|49=ABC|52=20120309-16:54:02|56=TT_ORDER|96=12345678|98=0|108=60|141=Y|10=248";
    Fix::visit(frame, std::strlen(frame), MyVisitor());
}
