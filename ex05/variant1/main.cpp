#include <set>
#include <list>
#include <vector>
#include <random>
#include <cassert>

#include "bfs.hpp"
#include "bfs_std_function.hpp"
#include "bfs_visitor.hpp"
#include "../../hw01/benchmark_tool.hpp"

template< typename G >
std::pair< int, int > bfs( G &graph )
{
    std::list< int > open;
    std::set< int > closed;

    graph.initials( [&]( auto v ) { open.push_back( v ); } );
    int edges = -1, vertices = 0;

    while ( !open.empty() )
    {
        auto v = open.front();
        open.pop_front();

        ++ edges;
        if ( closed.count( v ) )
            continue;

        closed.insert( v );
        ++ vertices;
        graph.edges( v, [&]( auto v ) { open.push_back( v ); } );
    }

    return std::make_pair( vertices, edges );
}

class MyEdgeVisitor : public EdgeVisitor
{
private:
    std::list< int >& m_open;

public:
    MyEdgeVisitor(std::list< int >& open) : m_open(open)
    {
    }

    void visit(int edge) override
    {
        m_open.push_back(edge);
    }
};

template< typename G >
std::pair< int, int > bfsVisitor( G &graph )
{
    std::list< int > open;
    std::set< int > closed;
    MyEdgeVisitor myEdgeVisitor(open);

    graph.initials( [&]( auto v ) { open.push_back( v ); } );
    int edges = -1, vertices = 0;

    while ( !open.empty() )
    {
        auto v = open.front();
        open.pop_front();

        ++ edges;
        if ( closed.count( v ) )
            continue;

        closed.insert( v );
        ++ vertices;
        graph.edges( v, myEdgeVisitor);
    }

    return std::make_pair( vertices, edges );
}

struct benchmark_data
{
    int vertices;
    int edges;
};

void benchmark_template(void* data)
{
    const benchmark_data& benchmarkData = *(benchmark_data*)data;
    Random r( benchmarkData.vertices, benchmarkData.edges );
    Benchmarking::size_info("vertices=" + std::to_string(benchmarkData.vertices) + ", edges=" + std::to_string(benchmarkData.edges));
    Benchmarking::start();
    auto b = bfs( r );
    Benchmarking::stop();
    assert( b.first == benchmarkData.vertices );
    assert( b.second == benchmarkData.edges );
}

void benchmark_std_function(void* data)
{
    const benchmark_data& benchmarkData = *(benchmark_data*)data;
    RandomStdFunction rRandom( benchmarkData.vertices, benchmarkData.edges );
    Benchmarking::size_info("vertices=" + std::to_string(benchmarkData.vertices) + ", edges=" + std::to_string(benchmarkData.edges));
    Benchmarking::start();
    auto b = bfs( rRandom );
    Benchmarking::stop();
    assert( b.first == benchmarkData.vertices );
    assert( b.second == benchmarkData.edges );
}

void benchmark_visitor(void* data)
{
    const benchmark_data& benchmarkData = *(benchmark_data*)data;
    RandomVisitor rVisitor( benchmarkData.vertices, benchmarkData.edges );
    Benchmarking::size_info("vertices=" + std::to_string(benchmarkData.vertices) + ", edges=" + std::to_string(benchmarkData.edges));
    Benchmarking::start();
    auto b = bfsVisitor( rVisitor );
    Benchmarking::stop();
    assert( b.first == benchmarkData.vertices );
    assert( b.second == benchmarkData.edges );
}


int main(int argc, char** argv)
{
    Benchmarking::init(argc, argv);

    benchmark_data benchmarkData{ 10000, 200000 };
    BENCHMARKING_RUN(benchmark_template, &benchmarkData);
    BENCHMARKING_RUN(benchmark_std_function, &benchmarkData);
    BENCHMARKING_RUN(benchmark_visitor, &benchmarkData);

    return 0;
}
