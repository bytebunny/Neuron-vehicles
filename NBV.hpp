/*!
The robot vehicle class
========================

Neuron based vehicles or N Braitenberg vehicles or New brain vehicle

Contains, classess for things, eyes, and binoculus. Monoculus dropped for the moment in favour of a vestigial right eye.

*/

#ifndef BRAITVEHICLE_H
#define BRAITVEHICLE_H

#include <iostream>
#include <sstream>
#include <list>		// list class library
#include <string>
#include "Coordmatrix.hpp"
#include "bvglobalini.hpp"
#include "constants.hpp"
#include "vehicle_types.hpp"

using std::cout;
using std::endl;
extern int globalID;
extern readinifile GS; //global singleton - initialised from a file.



// Light colour: Could do this as a one to one relationship to the photoreceptors, perhaps in a future version
// in the interum  yellow=red+green, cyan=blue+green, magenta=red+blue, orange=red+yellow!
enum{OFF,RED,GREEN,BLUE,YELLOW,CYAN,MAGENTA,WHITE};//light colour

/// All things must have things that attributes an id, colour,
/// halfwidth and type. Mainly used by the eyes so perhaps should also
/// have luminosity or be adapted by shadows.
/// The Thing class was intended to be inherited by Binocului, Monocului and
/// Lights however that looks impractical.
/// requires extern globalID
/// Might be redundant since it is not easy doing a list of things of different classes.
class Thing{
public:
  int id; /*!< unique id for each vehicle */
  int lcolour;	//Light colour  ie red blue green, cerise, aquamarine, turquoise, OFF 
  int vehicletype; // was bovatype, vehicleTypes::fixed_light etc
  int nCollisions; // number of collisions
  float halfwidth; //halfwidth - this is a typical dimension and should relate to the impression on the eye (dim)
  std::string name; // name (will use to display a label for  the vehicle
  int getid(){return(id);}// unique id
  //int getname(){}
  inline int colour(){return(lcolour);}/// return light colour
  inline void setcolour(int col){lcolour=col;}/// set light colour
  friend int renumberthing();
  //----------------------------constructors-----------------
  Thing(){
    nCollisions=0;
    halfwidth=.15;
    id=globalID++;
    lcolour=OFF;
  }
};

//-------------------------------------------------------------
/// Eye class
/// cones Blue, green, red - currently no rods 
/// Simple eye would have num receptors (in each class), total angle of reception, 
/// Complex eye would define each receptor.
class Eye{// eye has a number of receptors, 
  int nreceptors;// this may be redundant since it is also the size of the vectors.
public:
  std::vector<float> BluePR,Bleft,Bright;  /// blue - S-cones
  std::vector<float> GreenPR,Gleft,Gright; /// green - M-cones
  std::vector<float> RedPR,Rleft,Rright; /// Red Yellow - L-cones
  float theta;// eye angle
public:
  Coordmatrix Aeye;
  /// uniform distribution of receptors between +- angle
  void inituniform(int n,float ang){
    nreceptors=n;
    if (n==1){// single receptor
      Bleft[0]=Gleft[0]=Rleft[0]=ang;
      Bright[0]=Gright[0]=Rright[0]=-ang;
    }else if(n==2){// good for a monoculus
      Bleft[0]=Gleft[0]=Rleft[0]=ang;
      Bright[0]=Gright[0]=Rright[0]=Bleft[1]=Gleft[1]=Rleft[1]=0;
      Bright[1]=Gright[1]=Rright[1]=-ang;
    }else if(n==3){
      Bleft[0]=Gleft[0]=Rleft[0]=ang;
      Bright[0]=Gright[0]=Rright[0]=Bleft[1]=Gleft[1]=Rleft[1]=ang/3;
      Bright[1]=Gright[1]=Rright[1]=Bleft[2]=Gleft[2]=Rleft[2]=-ang/3;
      Bright[2]=Gright[2]=Rright[2]=-ang;
    }else{
      cout << "eye error"<<endl;
    }
  }
  /*  void set EyeDiv(float ltheta){
    theta=ltheta;
    Aeye.trans(ltheta,0.0,0.0);
    }*/

//Could name this setEyeAngle (and let the # arguments determine what is called)
  void setEyeDiv(float ltheta,float x,float y){
    theta=ltheta;
    Aeye.trans(ltheta,x,y);
  }
/// update theta and the rotation elements of A. See also setEyeDiv
  inline void setEyeAngle(float th){
    theta=th;
    Aeye.trans(th);// leaves position column untouched.
  }
  inline float getEyeAngle(){return theta;}

public:
  void clearRetina(){
    for (int ii = 0; ii < BluePR.size();ii++){
      BluePR.at(ii)=0;
      GreenPR.at(ii)=0;
      RedPR.at(ii)=0;
    }
  }

  /// computes the impression on the receptors given by an object in
  /// eye space with a dimension dim.
  //
  /// process all items that could be seen.
  /// Not particularly calibrated so will need some scaling to work
  /// two of the 12 comparisons are redundant, since we know d>c and b>a, but are left to retain the symmetry!
  void see(Coordmatrix & P, std::vector<int> & lightcols,float dim){
    clearRetina();
    for (int ii = 0; ii < lightcols.size();ii++){
      float x= P.getval(0,ii);
      float y= P.getval(1,ii);
      float ptheta=atan2(y,x);
      float intensity=1/(x*x+y*y);        
      //float gamma = sqrt(dim*dim*intensity);
      float gamma = dim*sqrt(intensity);// half the visual angle at the eye
      float b=ptheta+gamma;// this values can go above PI : no worries.
      float a=ptheta-gamma;// this values can go below -PI : no worries.
      float d,c;
      //cout << "a="<<a << "b="<<b<<"theta="<<ptheta<<"gamma="<<gamma<<endl;
      //cout << a << ","<<b<<","<<ptheta<<","<<gamma<<endl;
      //if(b>3.1);b=3.1;
      //if(a<-3.1);a=-3.1;

      if (P.getval(2,ii)!=1)cout << "warning check Pgetval"<<endl;
      //cout <<"theta="<<theta*180/constants::pi<<endl;
      //cout <<"gamma="<<gamma*180/constants::pi<<endl;

      for (int jj=0;jj<nreceptors;jj++){     
      //int jj=0;
	switch(lightcols[ii]){//Is the light is shining? If so what colour.
	case OFF://Ignore
	//cout <<ii<<" is off"<<endl;
	  break;
	case BLUE:
	  d=Bleft[jj];
	  c=Bright[jj];
	  if((d>b)&&(b>c)&&(c>a)) BluePR[jj] =BluePR.at(jj)+8*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) BluePR[jj] =BluePR.at(jj)+8*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) BluePR[jj] =BluePR.at(jj)+8*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) BluePR[jj] =BluePR.at(jj)+8*(d-a)*intensity;
	  break;
	case RED:
	  d=Rleft[jj];
	  c=Rright[jj];
	  if((d>b)&&(b>c)&&(c>a)) RedPR[jj] =RedPR.at(jj)+8*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) RedPR[jj] =RedPR.at(jj)+8*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) RedPR[jj] =RedPR.at(jj)+8*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) RedPR[jj] =RedPR.at(jj)+8*(d-a)*intensity;
	  break;
	case GREEN:
	  d=Gleft.at(jj);
	  c=Gright.at(jj);
	  if((d>b)&&(b>c)&&(c>a)) GreenPR[jj] =GreenPR.at(jj)+8*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) GreenPR[jj] =GreenPR.at(jj)+8*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) GreenPR[jj] =GreenPR.at(jj)+8*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) GreenPR[jj] =GreenPR.at(jj)+8*(d-a)*intensity;
	  break;
	case CYAN://Blue + green 
	  d=Gleft[jj];
	  c=Gright[jj];
	  if((d>b)&&(b>c)&&(c>a)) GreenPR[jj] =GreenPR.at(jj)+4*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) GreenPR[jj] =GreenPR.at(jj)+4*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) GreenPR[jj] =GreenPR.at(jj)+4*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) GreenPR[jj] =GreenPR.at(jj)+4*(d-a)*intensity;
	  
	  d=Bleft[jj];
	  c=Bright[jj];
	  if((d>b)&&(b>c)&&(c>a)) BluePR[jj] =BluePR.at(jj)+4*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) BluePR[jj] =BluePR.at(jj)+4*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) BluePR[jj] =BluePR.at(jj)+4*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) BluePR[jj] =BluePR.at(jj)+4*(d-a)*intensity;
	
	  break;
	}
	if (GS.fechner){
	  RedPR[jj]=log(1+RedPR[jj]);// so 0->0
	  GreenPR[jj]=log(1+GreenPR[jj]);
	  BluePR[jj]=log(1+BluePR[jj]);
	}else{
	  if (RedPR[jj]>5000)RedPR[jj]=5000;
	  if (GreenPR[jj]>5000)GreenPR[jj]=5000;
	  if (BluePR[jj]>5000)BluePR[jj]=5000;
	}
      }//end:for jj
    }//end:for ii
    //cout << "-"<<GreenPR[0]<<endl;
  }//end: see

/*----------------------------constructors-----------------*/
  /// Constructor needs a location of the eye, the number and
  /// location of the receptors.
  Eye(const float ltheta)
    :BluePR(6),Bleft(6),Bright(6),GreenPR(6),Gleft(6),Gright(6),RedPR(6),Rleft(6),Rright(6),Aeye(3,3){
    theta=ltheta;
    Aeye.trans(ltheta,0.0,0.0);
    inituniform(1,60*constants::pi/180);
  }
};









// Providing we cycle through a single list there does not seem to be
// a place for monoculus (or indeed M.Speculatrix) other than
// as adaptions of Binoculus.
// Placeholder
//class Monoculus: public Thing{
//};

#endif
