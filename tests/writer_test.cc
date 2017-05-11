#include "gtest/gtest.h"

#include <vector>
#include <unordered_map>

#define SOH_CHARACTER '|'

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v44.h>
#include <fixpp/writer.h>

template<typename Header>
Header createHeader()
{
    Header header;
    Fixpp::set<Fixpp::Tag::SenderCompID>(header, "SNDR");
    Fixpp::set<Fixpp::Tag::TargetCompID>(header, "TRGT");
    Fixpp::set<Fixpp::Tag::MsgSeqNum>(header, 1);
    Fixpp::set<Fixpp::Tag::SendingTime>(header, std::time(nullptr));

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

    Fixpp::Writer writer;
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
    Fixpp::v42::Message::Heartbeat heartbeat;
    Fixpp::set<Fixpp::Tag::TestReqID>(heartbeat, "TestReq");

    auto header = createHeader<Fixpp::v42::Header>();
    check(header, heartbeat, { { 8, "FIX.4.2" }, { 49, "SNDR" }, { 56, "TRGT" }, { 35, "0" }, { 112, "TestReq" } });
}

TEST(writer_test, should_write_heartbeat_message_44)
{
    Fixpp::v44::Message::Heartbeat heartbeat;
    Fixpp::set<Fixpp::Tag::TestReqID>(heartbeat, "TestReq");

    auto header = createHeader<Fixpp::v44::Header>();

    check(header, heartbeat, { { 8, "FIX.4.4" } });
}

TEST(writer_test, should_write_repeating_groups)
{
    Fixpp::v42::Message::Logon logon;
    Fixpp::set<Fixpp::Tag::EncryptMethod>(logon, 0);
    Fixpp::set<Fixpp::Tag::HeartBtInt>(logon, 60);

    auto group = Fixpp::createGroup<Fixpp::Tag::NoMsgTypes>(logon, 2);

    auto instance1 = group.instance();
    Fixpp::set<Fixpp::Tag::RefMsgType>(instance1, "TESTREF1");
    Fixpp::set<Fixpp::Tag::MsgDirection>(instance1, 'S');
    group.add(instance1);

    auto instance2 = group.instance();
    Fixpp::set<Fixpp::Tag::RefMsgType>(instance2, "TESTREF2");
    Fixpp::set<Fixpp::Tag::MsgDirection>(instance2, 'S');
    group.add(instance2);

    auto header = createHeader<Fixpp::v42::Header>();

    check(header, logon, { { 35, "A" }, { 98, "0" }, { 108, "60" }, { 384, "2" } });
}

TEST(writer_test, should_write_nested_repeating_groups)
{
    Fixpp::v44::Message::MarketDataSnapshot snapshot;
    Fixpp::set<Fixpp::Tag::MDReqID>(snapshot, "1709");
    Fixpp::set<Fixpp::Tag::Symbol>(snapshot, "AUD/CAD");

    auto underlyingGroups = Fixpp::createGroup<Fixpp::Tag::NoUnderlyings>(snapshot, 1);
    auto underlying0 = underlyingGroups.instance();

    Fixpp::set<Fixpp::Tag::UnderlyingProduct>(underlying0, 1);
    auto underlyingSecurityAltIDs = Fixpp::createGroup<Fixpp::Tag::NoUnderlyingSecurityAltID>(underlying0, 1);
    auto underlyingSecurityAltID0 = underlyingSecurityAltIDs.instance();

    Fixpp::set<Fixpp::Tag::UnderlyingSecurityAltID>(underlyingSecurityAltID0, "TESTID");
    Fixpp::set<Fixpp::Tag::UnderlyingSecurityAltIDSource>(underlyingSecurityAltID0, "TESTSOURCE");
    underlyingSecurityAltIDs.add(underlyingSecurityAltID0);
    Fixpp::set<Fixpp::Tag::UnderlyingSymbol>(underlying0, "AUD/CAD");

    underlyingGroups.add(underlying0);

    Fixpp::set<Fixpp::Tag::CorporateAction>(snapshot, 'D');

    auto mdEntries = Fixpp::createGroup<Fixpp::Tag::NoMDEntries>(snapshot, 2);
    auto mdEntry0 = mdEntries.instance();
    Fixpp::set<Fixpp::Tag::MDEntryType>(mdEntry0, '0');
    Fixpp::set<Fixpp::Tag::MDEntrySize>(mdEntry0, 500000.0);
    Fixpp::set<Fixpp::Tag::MDEntryDate>(mdEntry0, "20170103");
    Fixpp::set<Fixpp::Tag::QuoteEntryID>(mdEntry0, "02z00000hdi:A");
    mdEntries.add(mdEntry0);

    auto mdEntry1 = mdEntries.instance();
    Fixpp::set<Fixpp::Tag::MDEntryType>(mdEntry1, '1');
    Fixpp::set<Fixpp::Tag::MDEntrySize>(mdEntry1, 500000.0);
    Fixpp::set<Fixpp::Tag::MDEntryDate>(mdEntry1, "20170103");
    Fixpp::set<Fixpp::Tag::QuoteEntryID>(mdEntry1, "02z00000hdi:A");
    mdEntries.add(mdEntry1);

    auto header = createHeader<Fixpp::v44::Header>();

    Fixpp::Writer writer;
    std::cout << writer.write(header, snapshot) << std::endl;
}
