#include <chrono>
#include <fstream>

#define SOH_CHARACTER '|'

#include <fixpp/versions/v42.h>
#include <fixpp/visitor.h>
#include <fixpp/dsl.h>

size_t total = 0;

using MyTag1 = Fixpp::TagT<11325, Fixpp::Type::Int>;
using MyTag2 = Fixpp::TagT<537, Fixpp::Type::Int>;
using MyQuote = Fixpp::ExtendedMessage<Fixpp::v42::Message::Quote, Fixpp::Tag::QuoteRequestType, Fixpp::Tag::SettlmntTyp, MyTag1, MyTag2>;

struct VisitRules : public Fixpp::VisitRules
{
    using Overrides = OverrideSet<
        Override<Fixpp::v42::Message::Quote, As<MyQuote>>
    >;
    using Dictionary = Fixpp::v42::Spec::Dictionary;

    static constexpr bool ValidateChecksum = false;
    static constexpr bool ValidateLength = false;
    static constexpr bool StrictMode = false;
    static constexpr bool SkipUnknownTags = false;
};

struct Visitor : public Fixpp::StaticVisitor<void>
{
    void operator()(const Fixpp::v42::Header::Ref&, const MyQuote::Ref&)
    {
        ++total;
    }

    template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
    {
    }
};

std::vector<std::string> readFrames(const char* file)
{
    std::ifstream in(file);
    std::vector<std::string> result;

    if (!in)
    {
        std::cerr << "Could not open '" << file << "'" << std::endl;
    }
    std::string line;

    while (std::getline(in, line))
    {
        result.push_back(std::move(line));
    }

    return result;
}

int main()
{
    auto frames = readFrames("tests/data/fix42.log");

    auto start = std::chrono::system_clock::now();

    Visitor visitor;

    for (const auto& frame: frames)
    {
        Fixpp::visit(frame.c_str(), frame.size(), visitor, VisitRules());
    }

    auto end = std::chrono::system_clock::now();

    auto totalUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Parsed " << total << "/" << frames.size() << " frames in " << totalUs.count() << "us\n";

    auto frameLatencyUs = std::chrono::duration_cast<std::chrono::microseconds>(totalUs) / static_cast<double>(frames.size());
    std::cout << "-> " << frameLatencyUs.count() << "us / frame\n";
}
