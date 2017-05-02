#include <benchmark/benchmark.h>
#include <random>

#include <fixpp/tag.h>
#include <fixpp/dsl.h>
#include <fixpp/writer.h>
#include <fixpp/versions/v42.h>

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
        Fixpp::v42::Message::NewOrderSingle orderSingle;
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

    Fixpp::v42::Message::NewOrderSingle orderSingle;

    Fixpp::set<Fixpp::Tag::Symbol>(orderSingle, "BHP");
    Fixpp::set<Fixpp::Tag::Side>(orderSingle, '1');
    Fixpp::set<Fixpp::Tag::HandlInst>(orderSingle, '1');

    Fixpp::set<Fixpp::Tag::OrdType>(orderSingle, '2');
    Fixpp::set<Fixpp::Tag::TimeInForce>(orderSingle, '4');
    Fixpp::set<Fixpp::Tag::TransactTime>(orderSingle, t);

    while (state.KeepRunning())
    {
        auto id = makeId(i++);
        Fixpp::set<Fixpp::Tag::ClOrdID>(orderSingle, id + "-50000");

        Fixpp::set<Fixpp::Tag::OrderQty>(orderSingle, intDis(gen));
        Fixpp::set<Fixpp::Tag::Price>(orderSingle, realDis(gen));
    }
}

BENCHMARK(SetFieldsNewOrderSingleBenchmark);

static void WriteNewOrderSingleBenchmark(benchmark::State& state)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> intDis(1, 10000);
    std::uniform_real_distribution<> realDis(1.0, 500.0);
    Fixpp::v42::Message::NewOrderSingle orderSingle;

    std::time_t t = std::time(nullptr);

    Fixpp::set<Fixpp::Tag::Symbol>(orderSingle, "BHP");
    Fixpp::set<Fixpp::Tag::Side>(orderSingle, '1');
    Fixpp::set<Fixpp::Tag::HandlInst>(orderSingle, '1');

    Fixpp::set<Fixpp::Tag::OrdType>(orderSingle, '2');
    Fixpp::set<Fixpp::Tag::TimeInForce>(orderSingle, '4');
    Fixpp::set<Fixpp::Tag::TransactTime>(orderSingle, t);

    Fixpp::v42::Header header;
    Fixpp::set<Fixpp::Tag::SenderCompID>(header, "TEX_DLD");
    Fixpp::set<Fixpp::Tag::TargetCompID>(header, "DLD_TEX");
    Fixpp::set<Fixpp::Tag::SendingTime>(header, t);

    size_t i = 0;

    Fixpp::Writer writer;

    while (state.KeepRunning())
    {
        Fixpp::set<Fixpp::Tag::OrderQty>(orderSingle, intDis(gen));
        Fixpp::set<Fixpp::Tag::Price>(orderSingle, realDis(gen));

        auto id = makeId(i++);
        Fixpp::set<Fixpp::Tag::ClOrdID>(orderSingle, id + "-50000");

        writer.write(header, orderSingle);
    }
}

BENCHMARK(WriteNewOrderSingleBenchmark);

BENCHMARK_MAIN();
