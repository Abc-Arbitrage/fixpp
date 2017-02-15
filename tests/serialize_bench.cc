#include <benchmark/benchmark.h>
#include <random>

#include <fixpp/tag.h>
#include <fixpp/dsl.h>
#include <fixpp/writer.h>
#include <fixpp/versions/v42.h>

using namespace Fix;

std::string makeId(size_t index)
{
    char idBuffer[20];
    memset(idBuffer, 0, sizeof idBuffer);

    std::snprintf(idBuffer, sizeof idBuffer, "%s%lu", "ord", index);
    return std::string(idBuffer);
}

static void CreateNewOrderSingleBenchmark(benchmark::State& state)
{
    while (state.KeepRunning())
        Fix::v42::Message::NewOrderSingle orderSingle;
}

BENCHMARK(CreateNewOrderSingleBenchmark);

static void MakeUniqueIdBenchmark(benchmark::State& state)
{
    size_t i = 0;
    while (state.KeepRunning())
    {
        makeId(i++);
    }
}

BENCHMARK(MakeUniqueIdBenchmark);

static void SetFieldsNewOrderSingleBenchmark(benchmark::State& state)
{
    size_t i = 0;

    std::time_t t = std::time(nullptr);

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> intDis(1, 10000);
    std::uniform_real_distribution<> realDis(1.0, 500.0);

    Fix::v42::Message::NewOrderSingle orderSingle;

    Fix::set<Tag::Symbol>(orderSingle, "BHP");
    Fix::set<Tag::Side>(orderSingle, '1');
    Fix::set<Tag::HandlInst>(orderSingle, '1');

    Fix::set<Tag::OrdType>(orderSingle, '2');
    Fix::set<Tag::TimeInForce>(orderSingle, '4');
    Fix::set<Tag::TransactTime>(orderSingle, t);

    while (state.KeepRunning())
    {
        auto id = makeId(i++);
        Fix::set<Tag::ClOrdID>(orderSingle, id + "-50000");

        Fix::set<Tag::OrderQty>(orderSingle, intDis(gen));
        Fix::set<Tag::Price>(orderSingle, realDis(gen));
    }
}

BENCHMARK(SetFieldsNewOrderSingleBenchmark);

static void WriteNewOrderSingleBenchmark(benchmark::State& state)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> intDis(1, 10000);
    std::uniform_real_distribution<> realDis(1.0, 500.0);
    Fix::v42::Message::NewOrderSingle orderSingle;

    std::time_t t = std::time(nullptr);

    Fix::set<Tag::Symbol>(orderSingle, "BHP");
    Fix::set<Tag::Side>(orderSingle, '1');
    Fix::set<Tag::HandlInst>(orderSingle, '1');

    Fix::set<Tag::OrdType>(orderSingle, '2');
    Fix::set<Tag::TimeInForce>(orderSingle, '4');
    Fix::set<Tag::TransactTime>(orderSingle, t);

    Fix::v42::Header header;
    Fix::set<Tag::SenderCompID>(header, "TEX_DLD");
    Fix::set<Tag::TargetCompID>(header, "DLD_TEX");
    Fix::set<Tag::SendingTime>(header, t);

    size_t i = 0;

    Fix::Writer writer;

    while (state.KeepRunning())
    {
        Fix::set<Tag::OrderQty>(orderSingle, intDis(gen));
        Fix::set<Tag::Price>(orderSingle, realDis(gen));

        auto id = makeId(i++);
        Fix::set<Tag::ClOrdID>(orderSingle, id + "-50000");

        writer.write(header, orderSingle);
    }
}

BENCHMARK(WriteNewOrderSingleBenchmark);

BENCHMARK_MAIN();
