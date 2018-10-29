#ifndef BFS_HPP
#define BFS_HPP

#include <set>
#include <list>
#include <vector>
#include <random>
#include <cassert>

struct Random
{
    using State = int;
    using Label = int;

    std::vector< std::vector< int > > _succs;

    Random( int vertices, int edges, unsigned seed = 0 )
    {
        std::mt19937 rand{ seed };
        std::uniform_int_distribution< int > dist( 1, vertices );
        _succs.resize( vertices + 1 );
        std::set< int > connected;
        int last = 1;

        /* first connect everything */
        while ( int( connected.size() ) < vertices ) {
            int next = dist( rand );
            if ( connected.count( next ) )
                continue;
            _succs[ last ].push_back( next );
            connected.insert( next );
            last = next;
            -- edges;
        }

        /* add more edges at random */
        while ( edges > 0 ) {
        next:
            int from = dist( rand );
            int to = dist( rand );
            for ( auto c : _succs[ from ] )
                if ( to == c )
                    goto next;
            _succs[ from ].push_back( to );
            -- edges;
        }
    }

    template< typename Y >
    void edges( int from, Y yield );

    template< typename Y >
    void initials( Y yield )
    {
        yield( 1 );
    }
};

#endif // BFS_HPP
