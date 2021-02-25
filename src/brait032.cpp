/* 
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 */

/*! \mainpage Neuron controlled vehicles

You are welcome to modify and use this code for research, teaching or
any non critical applications.  Please credit the origin (a web
address is sufficient  http://www.reading.ac.uk/~shshawin ). If you make any
inspired changes and are happy for these to be published on the web
please let me know so that I can update the program.

This code uses glut (freeglut) and is based on pre version 3.0 opengl.
An Object Oriented wrapper is provided by glutMaster although not
strictly necessary.

Copyright William Harwin 2002-2015
\author W.S.Harwin (w.s.harwin at reading.ac.uk)

\section SECA User Commands


See also brait.ini for start up conditions 

\section SECB Proposed features and bugs

(Oh the power of the comma)

Need to still check the eye model, there may be light leaking in from the back. [2/15: seems ok and may only be a problem with eyes that have receptors over more than 360 degress!]


Clean up the dynamics/kinematics

Have an array of eyes. Possibly need a vector of receptor values
(cones/proprioceptprs etc), of actuators (agonist and antagonists),
and iternal states for the 'design a brain' extension.


Add the 'design a brain' facilities.



Other sensors eg a collision, wheel proprioception, and a fatigue
function (all sensed by a neuron of course)

Calculate the actual loop-time [2/15: Now implimented, available with
the 'i' command].  Start using this as the simulation time.


\section SECC Wishful thinking

Would be nice to get glutSpecialFunc working

would be nice to get a webgl version/variant?

Add obsticles to the arena.

Add a Morris water maze

*/

/*! \page intro Introduction


The programming style is functional rather than elegant and I have no doubt 
missed out on some of the subtleties of C++ programming. If you have a better
way of doing things, please advise - or better still please implement.


*/



/*
@todo Camera on the vehicle [done, although would be useful if it saw what the animal saw]
@todo Vectors to lights
@todo behaviours

Learning/optimization
*/

#include"constants.hpp"
#include"vehicle_types.hpp"

//#include <algorithm>    // std::remove_if
#include <fstream>
//#include <cstring>
#include <sstream>
#include <string>
#include <iostream>
#include <list>		// list class library
#include "glutMaster.hpp"
#include "Coordmatrix.hpp"  // matricies for coordinate transforms and data logging
#include "Binoculus.hpp"  // the vehicle class
#include "bvglobalini.hpp" // for initialising variables from bv.ini
#include "NRWindow.hpp"

using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::iterator;
using std::list;
using std::ofstream;
using std::string;

// --------------------Globals --------------
//list<Binoculus> bvl;
int globalID=1;//unique id
//string globalkbuf;
//int globaltime=0; // experimental for the moment.
//int globallooptime=0;
readinifile GS; //global singleton - initialised from a file.
bool aerialview=true;
int viewwidth=820;
int viewheight=680;
//bool movetoggle=false;


// Main function for the program
int main(int argc, char *argv[])
{
  char const * filename="bv.ini";

  if (GS.ini(filename,0))
    cout << "Warning: could not open bv.ini, using defaults "<<endl;
  GS.inspect();

  // Output the list values, by repeatedly getting the item from
  // the "front" of the list, outputting it, and removing it
  // from the front of the list.

  Binoculus Alice("Alice",-constants::pi/4,0,0,GREEN);
  Binoculus April("April",0.523599,-.85,-.85,GREEN); //30degrees

  NRWindow::globalBvl.push_back (Alice);
  NRWindow::globalBvl.push_back (April);

  NRWindow::globalNVselect = NRWindow::globalBvl.begin();/// Need to keep at least one item in the list

  char windowtitle[]="Neuro robots";

  GlutMaster * glutMaster   = new GlutMaster();

  NRWindow * Window = new NRWindow(glutMaster,
    		             viewwidth, viewheight,    // width, height
    		             10, 0,    // initPosition (x,y)
    		             windowtitle); // title

   glutMaster->CallGlutMainLoop();

  return EXIT_SUCCESS;
}
