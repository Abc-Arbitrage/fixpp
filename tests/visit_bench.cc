#include <benchmark/benchmark.h>

#define SOH_CHARACTER '|'

#include <fixpp/tag.h>
#include <fixpp/dsl.h>
#include <fixpp/versions/v42.h>
#include <fixpp/visitor.h>

using MyTag1 = Fixpp::TagT<11325, Fixpp::Type::Int>;
using MyTag2 = Fixpp::TagT<537, Fixpp::Type::Int>;
using MyQuote = Fixpp::ExtendedMessage<Fixpp::v42::Message::Quote, Fixpp::Tag::QuoteRequestType, Fixpp::Tag::SettlmntTyp, MyTag1, MyTag2>;

struct MyVisitRules : public Fixpp::VisitRules
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

struct MyVisitor : public Fixpp::StaticVisitor<void>
{
    void operator()(const Fixpp::v42::Header::Ref&, const MyQuote::Ref&)
    {
    }

    template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
    {
    }
};

struct GetVisitor : public Fixpp::StaticVisitor<void>
{
    void operator()(const Fixpp::v42::Header::Ref&, const MyQuote::Ref& quote)
    {
        benchmark::DoNotOptimize(Fixpp::get<MyTag1>(quote));
        benchmark::DoNotOptimize(Fixpp::get<MyTag2>(quote));
        benchmark::DoNotOptimize(Fixpp::get<Fixpp::Tag::BidPx>(quote));
    }

    template<typename HeaderT, typename MessageT> void operator()(HeaderT, MessageT)
    {
    }
};

static void VisitCustomQuoteBenchmark(benchmark::State& state)
{
    const char *frame = "8=FIX.4.2|9=0225|35=S|49=FIXPROV|56=TRGT|34=1579321|52=20161230-11:05:36.052|115=TRGT|142=MRS|55=ZAR/JPY|60=20161230-11:05:36.052|63=0|64=20170105|117=d20052s3866|131=1276|132=8.525|133=8.547|134=1000000|135=1000000|303=2|537=1|11325=0|10=087|";

    const size_t size = std::strlen(frame);

    MyVisitor visitor;

    while (state.KeepRunning())
    {
        Fixpp::visit(frame, size, visitor, MyVisitRules()).otherwise([&](const Fixpp::ErrorKind& e) {
            auto errStr = e.asString();
            state.SkipWithError(errStr.c_str());
        });
    }

}

static void VisitCustomQuoteAndGetTagsBenchmark(benchmark::State& state)
{
    const char *frame = "8=FIX.4.2|9=0225|35=S|49=FIXPROV|56=TRGT|34=1579321|52=20161230-11:05:36.052|115=TRGT|142=MRS|55=ZAR/JPY|60=20161230-11:05:36.052|63=0|64=20170105|117=d20052s3866|131=1276|132=8.525|133=8.547|134=1000000|135=1000000|303=2|537=1|11325=0|10=087|";

    const size_t size = std::strlen(frame);

    GetVisitor visitor;

    while (state.KeepRunning())
    {
        Fixpp::visit(frame, size, visitor, MyVisitRules()).otherwise([&](const Fixpp::ErrorKind& e) {
            auto errStr = e.asString();
            state.SkipWithError(errStr.c_str());
        });
    }
}

static void VisitTagViewBenchmark(benchmark::State& state)
{
    const char *frame = "8=FIX.4.2|9=0225|35=S|49=FIXPROV|56=TRGT|34=1579321|52=20161230-11:05:36.052|115=TRGT|142=MRS|55=ZAR/JPY|60=20161230-11:05:36.052|63=0|64=20170105|117=d20052s3866|131=1276|132=8.525|133=8.547|134=1000000|135=1000000|303=2|537=1|11325=0|10=087|";

    const size_t size = std::strlen(frame);

    while (state.KeepRunning())
    {
        Fixpp::visitTagView<Fixpp::Tag::MsgType>(frame, size).otherwise([&](const Fixpp::ErrorKind& e) {
            auto errStr = e.asString();
            state.SkipWithError(errStr.c_str());
        });
    }
}

static void VisitTagBenchmark(benchmark::State& state)
{
    const char *frame = "8=FIX.4.2|9=0225|35=S|49=FIXPROV|56=TRGT|34=1579321|52=20161230-11:05:36.052|115=TRGT|142=MRS|55=ZAR/JPY|60=20161230-11:05:36.052|63=0|64=20170105|117=d20052s3866|131=1276|132=8.525|133=8.547|134=1000000|135=1000000|303=2|537=1|11325=0|10=087|";

    const size_t size = std::strlen(frame);

    while (state.KeepRunning())
    {
        Fixpp::visitTag<Fixpp::Tag::MsgType>(frame, size).otherwise([&](const Fixpp::ErrorKind& e) {
            auto errStr = e.asString();
            state.SkipWithError(errStr.c_str());
        });
    }
}

BENCHMARK(VisitCustomQuoteBenchmark);
BENCHMARK(VisitCustomQuoteAndGetTagsBenchmark);
BENCHMARK(VisitTagViewBenchmark);
BENCHMARK(VisitTagBenchmark);

BENCHMARK_MAIN();
