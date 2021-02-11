#ifndef BINOCULUS_H
#define BINOCULUS_H

#include "NBV.h"
#include "nrWindow.h"

class Binoculus: public Thing{
  float x[76];/// States are    \[ speed, \dot\theta,\theta, x, y \]
  float dotx[76];/// dot v, ddtheta, dtheta, dx, dy;
  Coordmatrix A;
  float BOverMass, BrOverInertia ;
  float mass, inertia;
  float B,  Brot;
  float Ml,Mr; // written to by think, inputs to the actuators.
  std::vector<float> BrainGains; // Tmp brain gains.


public:

  Eye righteye,lefteye;  // can debate if this should be private or not...

  Coordmatrix log;// default log
  Coordmatrix log2;// second log etc. etc.

public:
  inline float getth(){return(x[2]);}
  inline float getx(){return(x[3]);}
  inline float gety(){return(x[4]);}
  inline float getvelocity(){return(x[0]);}
  inline float getturnrate(){return(x[1]);}
  inline float getEyeDiv(){return(lefteye.theta);}// candidate for removal
  inline void resetstates(){x[0]=x[1]=x[5]=0;dotx[0]=dotx[1]=dotx[2]=dotx[3]=dotx[4]=dotx[5]=0;}
    inline int getid(){return(id);}
  //  friend void printWidthand( Box & box );
  void updateA(){A.trans(x[2],x[3],x[4]);}


  void updatelog(){ // may not need this method since, if logs are public, we can access directly!
    log.addcolumn(x[2],x[3],x[4]); // theta x y
  }
    void updatespecu();
    void updatestate();
    void look();
    void relocate(float xx, float yy);
    void think();
    void info();
    void setEyeDivergence(float angle,float x,float y);
    void setMIBs(float lM,float lI,float lB,float lBrot);

  friend class Coordmatrix;
  friend void eulerstep(Binoculus &nbv);
  friend void worldphysics(Binoculus &nbv);
  friend void boundarycollisions(Binoculus &nbv);
  friend void boundaryfolding(Binoculus &nbv);
  //----------------------------constructors-----------------
    Binoculus(const char * argname,float theta,float xx, float yy, int colour);
};

void writevehiclelogs( std::list<Binoculus>& globalBvl,
                       std::string fname, char var, int lnum );


#endif // BINOCULUS_H
