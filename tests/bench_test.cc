#include <chrono>
#include <fstream>

#pragma warning(disable:4503)

#include <fixpp/visitor.h>
#include <fixpp/dsl.h>

size_t total = 0;

using MyTag1 = Fix::TagT<11325, Fix::Type::Int>;
using MyTag2 = Fix::TagT<537, Fix::Type::Int>;
using MyQuote = Fix::ExtendedMessage<Fix::v42::Message::Quote, Fix::Tag::QuoteResponseType, Fix::Tag::SettlmntTyp, MyTag1, MyTag2>;

struct VisitRules : public Fix::VisitRules
{
    using Overrides = OverrideSet<
        Override<Fix::v42::Message::Quote, As<MyQuote>>
    >;

    static constexpr bool ValidateChecksum = false;
    static constexpr bool ValidateLength = false;
    static constexpr bool StrictMode = false;
};

struct Visitor
{
    void operator()(const Fix::v42::Header::Ref&, const MyQuote::Ref&)
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

    for (const auto& frame: frames)
    {
        Fix::visit(frame.c_str(), frame.size(), Visitor(), VisitRules());
    }

    auto end = std::chrono::system_clock::now();

    auto totalUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Parsed " << total << "/" << frames.size() << " frames in " << totalUs.count() << "us\n";

    auto frameLatencyUs = std::chrono::duration_cast<std::chrono::microseconds>(totalUs) / static_cast<double>(frames.size());
    std::cout << "-> " << frameLatencyUs.count() << "us / frame\n";
}
