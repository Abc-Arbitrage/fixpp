#include <benchmark/benchmark.h>

#include <fixpp/tag.h>
#include <fixpp/dsl.h>
#include <fixpp/versions/v42.h>
#include <fixpp/visitor.h>

using namespace Fix;

using MyTag1 = Fix::TagT<11325, Fix::Type::Int>;
using MyTag2 = Fix::TagT<537, Fix::Type::Int>;
using MyQuote = Fix::ExtendedMessage<Fix::v42::Message::Quote, Fix::Tag::QuoteResponseType, Fix::Tag::SettlmntTyp, MyTag1, MyTag2>;

struct MyVisitRules : public Fix::VisitRules
{
    using Overrides = OverrideSet<
        Override<Fix::v42::Message::Quote, As<MyQuote>>
    >;

    static constexpr bool ValidateChecksum = false;
    static constexpr bool ValidateLength = false;
    static constexpr bool StrictMode = false;
};

struct MyVisitor
{
    void operator()(const Fix::v42::Header::Ref&, const MyQuote::Ref&)
    {
    }

    template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
    {
    }
};

static void VisitCustomQuoteBenchmark(benchmark::State& state)
{
    const char *frame = "8=FIX.4.2|9=0225|35=S|49=FIXPROV|56=TRGT|34=1579321|52=20161230-11:05:36.052|115=TRGT|142=MRS|55=ZAR/JPY|60=20161230-11:05:36.052|63=0|64=20170105|117=d20052s3866|131=1276|132=8.525|133=8.547|134=1000000|135=1000000|303=2|537=1|11325=0|10=087|";

    const size_t size = std::strlen(frame);

    while (state.KeepRunning())
    {
        Fix::visit(frame, size, MyVisitor(), MyVisitRules());
    }

}

BENCHMARK(VisitCustomQuoteBenchmark);

BENCHMARK_MAIN();
