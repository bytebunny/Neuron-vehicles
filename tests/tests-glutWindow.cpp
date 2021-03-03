#include <catch2/catch.hpp>

#include "glutWindow.hpp"

// TEST_CASE( "Access functions of GlutWindow class", "[GlutWindow]" )
// {
//     GlutWindow gw{};

//     SECTION( "getter returns zero for the new object" )
//     {
//         REQUIRE( gw.GetWindowID() == 0 );
//     }
//     SECTION( "setter modifies what getter will return" )
//     {
//         gw.SetWindowID( 4 );
//         REQUIRE( gw.GetWindowID() == 4 );

//         gw.SetWindowID( 11 );
//         REQUIRE( gw.GetWindowID() == 11 );
//     }
// }

SCENARIO( "Objects of GlutWindow class have access functions", "[GlutWindow]" )
{
    GIVEN( "A new GlutWindow object" )
    {
        GlutWindow gw{};
        REQUIRE( gw.GetWindowID() == 0 );

        WHEN( "the setter is used" )
        {
            gw.SetWindowID( 4 );

            THEN( "the getter reflects the change" )
            {
                REQUIRE( gw.GetWindowID() == 4 );

                gw.SetWindowID( 11 );
                REQUIRE( gw.GetWindowID() == 11 );
            }
        }
    }
}
