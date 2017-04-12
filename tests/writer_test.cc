#include "gtest/gtest.h"

#include <vector>
#include <unordered_map>

#pragma warning(disable:4503)

#define SOH_CHARACTER '|'

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v44.h>
#include <fixpp/writer.h>

template<typename Header>
Header createHeader()
{
    Header header;
    Fix::set<Fix::Tag::SenderCompID>(header, "SNDR");
    Fix::set<Fix::Tag::TargetCompID>(header, "TRGT");
    Fix::set<Fix::Tag::MsgSeqNum>(header, 1);
    Fix::set<Fix::Tag::SendingTime>(header, std::time(nullptr));

    return header;
}

// TODO: find a way to correctly check repeating groups
template<typename Header, typename Message>
void check(const Header& header, const Message& message, std::initializer_list<std::pair<int, std::string>> values)
{
    auto split = [](const std::string& value, char delim)
    {
        std::vector<std::string> ret;
        std::string line;
        
        std::istringstream iss(value);
        while (std::getline(iss, line, delim))
        {
            ret.push_back(std::move(line));
        }

        return ret;
    };

    Fix::Writer writer;
    auto frame = writer.write(header, message);

    std::unordered_map<int, std::string> fields;
    auto tokens = split(frame, SOH_CHARACTER);

    for (const auto& token: tokens)
    {
        auto tagValue = split(token, '=');
        ASSERT_EQ(tagValue.size(), 2);
    
        fields.insert(std::make_pair(std::stoi(tagValue[0]), tagValue[1]));
    }

    for (const auto& value: values)
    {
        auto it = fields.find(value.first);
        ASSERT_EQ(it->second, value.second);
    }
}

TEST(writer_test, should_write_heartbeat_message_42)
{
    Fix::v42::Message::Heartbeat heartbeat;
    Fix::set<Fix::Tag::TestReqID>(heartbeat, "TestReq");

    auto header = createHeader<Fix::v42::Header>();
    check(header, heartbeat, { { 8, "FIX.4.2" }, { 49, "SNDR" }, { 56, "TRGT" }, { 35, "0" }, { 112, "TestReq" } });
}

TEST(writer_test, should_write_heartbeat_message_44)
{
    Fix::v44::Message::Heartbeat heartbeat;
    Fix::set<Fix::Tag::TestReqID>(heartbeat, "TestReq");

    auto header = createHeader<Fix::v44::Header>();

    check(header, heartbeat, { { 8, "FIX.4.4" } });
}

TEST(writer_test, should_write_repeating_groups)
{
    Fix::v42::Message::Logon logon;
    Fix::set<Fix::Tag::EncryptMethod>(logon, 0);
    Fix::set<Fix::Tag::HeartBtInt>(logon, 60);

    auto group = Fix::createGroup<Fix::Tag::NoMsgTypes>(logon, 2);

    auto instance1 = group.instance();
    Fix::set<Fix::Tag::RefMsgType>(instance1, "TESTREF1");
    Fix::set<Fix::Tag::MsgDirection>(instance1, 'S');
    group.add(instance1);

    auto instance2 = group.instance();
    Fix::set<Fix::Tag::RefMsgType>(instance2, "TESTREF2");
    Fix::set<Fix::Tag::MsgDirection>(instance2, 'S');
    group.add(instance2);

    auto header = createHeader<Fix::v42::Header>();

    check(header, logon, { { 35, "A" }, { 98, "0" }, { 108, "60" }, { 384, "2" } });
}

TEST(writer_test, should_write_nested_repeating_groups)
{
    Fix::v44::Message::MarketDataSnapshot snapshot;
    Fix::set<Fix::Tag::MDReqID>(snapshot, "1709");
    Fix::set<Fix::Tag::Symbol>(snapshot, "AUD/CAD");

    auto underlyingGroups = Fix::createGroup<Fix::Tag::NoUnderlyings>(snapshot, 1);
    auto underlying0 = underlyingGroups.instance();

    Fix::set<Fix::Tag::UnderlyingProduct>(underlying0, 1);
    auto underlyingSecurityAltIDs = Fix::createGroup<Fix::Tag::NoUnderlyingSecurityAltID>(underlying0, 1);
    auto underlyingSecurityAltID0 = underlyingSecurityAltIDs.instance();

    Fix::set<Fix::Tag::UnderlyingSecurityAltID>(underlyingSecurityAltID0, "TESTID");
    Fix::set<Fix::Tag::UnderlyingSecurityAltIDSource>(underlyingSecurityAltID0, "TESTSOURCE");
    underlyingSecurityAltIDs.add(underlyingSecurityAltID0);
    Fix::set<Fix::Tag::UnderlyingSymbol>(underlying0, "AUD/CAD");

    underlyingGroups.add(underlying0);

    Fix::set<Fix::Tag::CorporateAction>(snapshot, 'D');

    auto mdEntries = Fix::createGroup<Fix::Tag::NoMDEntries>(snapshot, 2);
    auto mdEntry0 = mdEntries.instance();
    Fix::set<Fix::Tag::MDEntryType>(mdEntry0, '0');
    Fix::set<Fix::Tag::MDEntrySize>(mdEntry0, 500000.0);
    Fix::set<Fix::Tag::MDEntryDate>(mdEntry0, "20170103");
    Fix::set<Fix::Tag::QuoteEntryID>(mdEntry0, "02z00000hdi:A");
    mdEntries.add(mdEntry0);

    auto mdEntry1 = mdEntries.instance();
    Fix::set<Fix::Tag::MDEntryType>(mdEntry1, '1');
    Fix::set<Fix::Tag::MDEntrySize>(mdEntry1, 500000.0);
    Fix::set<Fix::Tag::MDEntryDate>(mdEntry1, "20170103");
    Fix::set<Fix::Tag::QuoteEntryID>(mdEntry1, "02z00000hdi:A");
    mdEntries.add(mdEntry1);

    auto header = createHeader<Fix::v44::Header>();

    Fix::Writer writer;
    std::cout << writer.write(header, snapshot) << std::endl;
}
