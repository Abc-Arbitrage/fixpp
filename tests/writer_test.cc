#include "gtest/gtest.h"

#include <vector>
#include <unordered_map>

#define SOH_CHARACTER '|'

#include <fixpp/versions/v42.h>
#include <fixpp/versions/v44.h>
#include <fixpp/writer.h>
#include <fixpp/utils/time.h>

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

TEST(writer_test, should_write_bool_field)
{
    using Field = Fixpp::Field<Fixpp::Tag::PossDupFlag>;
    Field field;
    field.set(true);

    std::ostringstream oss;
    writeField(oss, field);

    ASSERT_EQ(oss.str(), "43=Y|");

    oss.str("");

    field.set(false);
    Fixpp::details::FieldWriter<Field>::write(oss, field);

    ASSERT_EQ(oss.str(), "43=N|");
}

TEST(writer_test, should_write_char_field)
{
    using Field = Fixpp::Field<Fixpp::Tag::OrdStatus>;
    Field field;
    field.set('4');

    std::ostringstream oss;
    writeField(oss, field);

    ASSERT_EQ(oss.str(), "39=4|");
}

TEST(writer_test, should_write_float_field)
{
    using Field = Fixpp::Field<Fixpp::Tag::Price>;
    Field field;

    field.set(17.35);

    std::ostringstream oss;
    writeField(oss, field);

    ASSERT_EQ(oss.str(), "44=17.35|");

    field.set(1234567.45);

    oss.str("");
    writeField(oss, field);

    ASSERT_EQ(oss.str(), "44=1234567.45|");

    field.set(1000000.0);

    oss.str("");
    writeField(oss, field);

    ASSERT_EQ(oss.str(), "44=1000000|");
}

TEST(writer_test, should_write_utc_timestamp_field)
{
    using Field = Fixpp::Field<Fixpp::Tag::SendingTime>;
    Field field;

    std::tm tm{};
    tm.tm_year = 117;
    tm.tm_mon = 4;
    tm.tm_mday = 16;
    tm.tm_hour = 13;
    tm.tm_min = 45;
    tm.tm_sec = 30;

    auto ts = mkgmtime(&tm);
    field.set(ts);

    std::ostringstream oss;
    writeField(oss, field);

    ASSERT_EQ(oss.str(), "52=20170516-13:45:30|");
}

TEST(writer_test, should_write_utc_timestamp_field_with_milliseconds)
{
    using Field = Fixpp::Field<Fixpp::Tag::SendingTime>;
    Field field;

    std::tm tm{};
    tm.tm_year = 117;
    tm.tm_mon = 4;
    tm.tm_mday = 16;
    tm.tm_hour = 13;
    tm.tm_min = 45;
    tm.tm_sec = 30;

    auto ts = mkgmtime(&tm);
    field.set(Fixpp::Type::UTCTimestamp::Time{ts, 123});

    std::ostringstream oss;
    writeField(oss, field);

    ASSERT_EQ(oss.str(), "52=20170516-13:45:30.123|");
}

TEST(writer_test, should_write_utc_timestamp_field_with_microseconds)
{
    using Field = Fixpp::Field<Fixpp::Tag::SendingTime>;
    Field field;

    std::tm tm{};
    tm.tm_year = 117;
    tm.tm_mon = 4;
    tm.tm_mday = 16;
    tm.tm_hour = 13;
    tm.tm_min = 45;
    tm.tm_sec = 30;

    auto ts = mkgmtime(&tm);
    field.set(Fixpp::Type::UTCTimestamp::Time{ts, 123, 456});

    std::ostringstream oss;
    writeField(oss, field);

    ASSERT_EQ(oss.str(), "52=20170516-13:45:30.123456|");
}

TEST(writer_test, should_write_int_field)
{
    using Field = Fixpp::Field<Fixpp::Tag::MsgSeqNum>;
    Field field;
    field.set(25);

    std::ostringstream oss;
    writeField(oss, field);

    ASSERT_EQ(oss.str(), "34=25|");
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

TEST(writer_test, should_compute_proper_length_and_checksum)
{
    Fixpp::v42::Message::Heartbeat heartbeat;
    Fixpp::set<Fixpp::Tag::TestReqID>(heartbeat, "TestReq");

    auto header = createHeader<Fixpp::v42::Header>();
    Fixpp::Writer writer;

    auto frame = writer.write(header, heartbeat);

    auto bodyLengthStartPos = frame.find("9=");
    auto bodyLengthEnd = frame.find(SOH_CHARACTER, bodyLengthStartPos);
    auto bodyLengthValueStart = bodyLengthStartPos + 2;

    auto bodyLength = frame.substr(bodyLengthValueStart, bodyLengthEnd - bodyLengthValueStart);
    auto extractedBodyLength = std::stoi(bodyLength);

    auto checksumStartPos = frame.find("10=");
    auto checksumEnd = frame.find(SOH_CHARACTER, checksumStartPos);
    auto checksumValueStart = checksumStartPos + 3;

    auto checksum = frame.substr(checksumValueStart, checksumEnd - checksumValueStart);
    auto extractedChecksum = std::stoi(checksum);

    auto expectedBodyLength = checksumStartPos - bodyLengthEnd - 1;

    size_t sum = 0;
    for (size_t i = 0; i < checksumStartPos; ++i)
    {
        sum += static_cast<size_t>(frame[i]);
    }
    
    auto expectedChecksum = sum % 256;

    ASSERT_EQ(expectedBodyLength, extractedBodyLength);
    ASSERT_EQ(expectedChecksum, extractedChecksum);
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

    std::tm entryDateTm{};
    entryDateTm.tm_year = 117; // 2017
    entryDateTm.tm_mon = 4;    // May
    entryDateTm.tm_mday = 16;
    Fixpp::Type::UTCDate::Date entryDate{mkgmtime(&entryDateTm)};

    std::tm entryTimeTm{};
    entryTimeTm.tm_hour = 15;
    entryTimeTm.tm_min = 10;
    entryTimeTm.tm_sec = 5;
    Fixpp::Type::UTCTimeOnly::Time entryTime{mkgmtime(&entryTimeTm), 100};

    auto mdEntries = Fixpp::createGroup<Fixpp::Tag::NoMDEntries>(snapshot, 2);
    auto mdEntry0 = mdEntries.instance();
    Fixpp::set<Fixpp::Tag::MDEntryType>(mdEntry0, '0');
    Fixpp::set<Fixpp::Tag::MDEntrySize>(mdEntry0, 500000.0);
    Fixpp::set<Fixpp::Tag::MDEntryDate>(mdEntry0, entryDate);
    Fixpp::set<Fixpp::Tag::MDEntryTime>(mdEntry0, entryTime);
    Fixpp::set<Fixpp::Tag::QuoteEntryID>(mdEntry0, "02z00000hdi:A");
    mdEntries.add(mdEntry0);

    auto mdEntry1 = mdEntries.instance();
    Fixpp::set<Fixpp::Tag::MDEntryType>(mdEntry1, '1');
    Fixpp::set<Fixpp::Tag::MDEntrySize>(mdEntry1, 500000.0);
    Fixpp::set<Fixpp::Tag::MDEntryDate>(mdEntry1, entryDate);
    Fixpp::set<Fixpp::Tag::MDEntryTime>(mdEntry0, entryTime);
    Fixpp::set<Fixpp::Tag::QuoteEntryID>(mdEntry1, "02z00000hdi:A");
    mdEntries.add(mdEntry1);

    auto header = createHeader<Fixpp::v44::Header>();

    Fixpp::Writer writer;
    std::cout << writer.write(header, snapshot) << std::endl;
}
