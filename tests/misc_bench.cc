#include <benchmark/benchmark.h>

#include <fixpp/tag.h>
#include <fixpp/dsl/details/lexical_cast.h>

static void BenchCustomUTCTimestampParsing(benchmark::State& state)
{
    const char* str = "20170511-15:06:30";
    const size_t size = std::strlen(str);

    while (state.KeepRunning())
    {
        Fixpp::details::LexicalCast<Fixpp::Type::UTCTimestamp>::cast(str, size);
    }
}

static void BenchStrptimeUTCTimestampParsing(benchmark::State& state)
{
    const char* str = "20170511-15:06:30";
    const size_t size = std::strlen(str);

    while (state.KeepRunning())
    {
        std::tm tm;
        strptime(str, "%Y%m%d-%H:%M:%S",  &tm);
        mkgmtime(&tm);
    }
}

BENCHMARK(BenchCustomUTCTimestampParsing);
BENCHMARK(BenchStrptimeUTCTimestampParsing);

BENCHMARK_MAIN();
