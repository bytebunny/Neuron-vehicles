#include <iostream>
#include "Binoculus.hpp"

/// Binoculus constructor
Binoculus::Binoculus(const char * argname,float theta,float xx, float yy, int colour)
    : righteye(-0.45), lefteye(0.45), A(3,3), BrainGains(9)
{// dawn robot weighs 390g+140g of batteries
    mass=0.4;inertia=.0005;// appears to be a problem when inertia is too small (16<B/I<166 (around 25)! ) ...(could be we are spinning)
    //B=.1;Brot=.00001;
    B=0.64;Brot=0.004;
    name=argname;
    vehicletype= vehicleTypes::fixed_light;
    x[0]=0;x[1]=0;x[2]=theta;x[3]=xx,x[4]=yy;
    lcolour=colour;
    A.trans(x[2],x[3],x[4]);
}

void Binoculus::updatespecu() // M. Speculatrix states
{
    // input is the photocell, outputs are steering rotation speed, and drive wheel speed
    float intensity=lefteye.GreenPR.at(0);
    float vs=GS.Speculatrix.at(2); //  constant forward velocity
    lefteye.inituniform(1,GS.Speculatrix[6]*constants::pi/180);
    //std::cout <<"intensity="<<intensity<<std::endl;
    if (intensity < GS.Speculatrix[0]){
      //lefteye.theta=lefteye.theta+GS.Speculatrix[1];
      //lefteye.Aeye.trans(lefteye.theta);// this is a bit of a hack, should have a routine to update the ht when you update theta
      lefteye.setEyeAngle(lefteye.theta+GS.Speculatrix[1]);

      //vs=vs/4;
      vs=vs*GS.Speculatrix[5];
    }

    //if (lefteye.GreenPR.at(0) > 0) std::cout << lefteye.GreenPR[0]<<std::endl;
    float alpha=lefteye.theta;
    //std::cout <<"alpha="<<alpha<<std::endl;
    //float alpha=GS.Speculatrix[6];
    float h=GS.Speculatrix[3];
    float vf=vs*cos(alpha);
    float omega=vs*sin(alpha)/h;

    dotx[0]=0;// steering wheel is rotating at a constant speed
    dotx[1]=0;
    dotx[2]=omega;
    dotx[3]=vf*cos(x[2]);
    dotx[4]=vf*sin(x[2]);

    //std::cout << "alpha="<<alpha<<std::endl;
}

void Binoculus::updatestate() /// for states 0,1,2 $\dot{x}=Ax$
{
    // inputs (limit to 5N - should probably reduce to 2/3)
    if(Ml>5)Ml=5;
    if(Ml<-5)Ml=-5;
    if(Mr>5)Mr=5;
    if(Mr<-5)Mr=-5;
    float F=(Ml+Mr);
    float T=(-Ml+Mr)*0.05;// implies 10 cm wheel base

    /// States =[ speed, \dot\theta,\theta, x, y ]
    // normalise theta (x[2]), allows easier checking of the orientation of the vehicle.
    while (x[2] > constants::pi)
        x[2]=x[2]-2*constants::pi;
    while(x[2]< -constants::pi)
        x[2]=x[2]+2*constants::pi;
    float af=B/mass;float at=Brot/inertia;


    dotx[0]=-af*x[0]+F/mass;
    dotx[1]=-at*x[1]+T/inertia;
    dotx[2]=x[1];
    dotx[3]=x[0]*cos(x[2]);
    dotx[4]=x[0]*sin(x[2]);
}

void Binoculus::look() /// update the receptors
{
    std::list<Binoculus>::iterator itr;
    std::vector<int> lightcolours;  /// colours for each vehicle
    Coordmatrix lightposns, bluel,greenl,redl;// yellowl,
    Coordmatrix A1inv(3,3),ALeyeinv(3,3),AReyeinv(3,3),ans;

    // computer the coordinate matrices needed
    A1inv=A;
    A1inv.inverse();
    ALeyeinv=lefteye.Aeye;ALeyeinv.inverse();
    AReyeinv=righteye.Aeye;AReyeinv.inverse();

    for (itr=NRWindow::globalBvl.begin(); itr != NRWindow::globalBvl.end(); itr++)
    {/// Check against every other vehicle
      if((*itr).id !=id)
      {// if not yourself
          lightposns.addcolumn((*itr).getx(),(*itr).gety(),1.0);
	      lightcolours.push_back((*itr).colour());
      }// end: if
    }//end:for

    if (lightposns.cols()>0)
    {
      //std::cout <<"---"<<name<<"---, id="<<id<<",";
      //std::cout << "id="<<id<<",";
      ans=(ALeyeinv*A1inv)*lightposns;
      lefteye.see(ans, lightcolours,0.15);// implies a dimension of 10cm (should make this the halfwidth)
      //std::cout<<"left eye="<<ans;
      ans=(AReyeinv*A1inv)*lightposns;
      righteye.see(ans, lightcolours,0.15);

      if(GS.logging)
      {
          log2.addcolumn( lefteye.BluePR[0], lefteye.GreenPR[0], lefteye.RedPR[0],
                          righteye.BluePR[0], righteye.GreenPR[0], righteye.RedPR[0] );
      }
      //std::cout <<std::endl;
      //std::cout <<"right eye="<<ans;
      //log2.addcolumn(id,ans.myValues[0],ans.myValues[1]); // log id x y
      //log2.addcolumn(id,righteye.BluePR[0],righteye.GreenPR[0],righteye.RedPR[0],ans.myValues[0],ans.myValues[1]); // log id
    }
    else
    {// Nothing to see, nothing to see, move along
      lefteye.clearRetina();
      righteye.clearRetina();
    }
}

void Binoculus::relocate(float xx, float yy) // magically move vehicle to x,y
{
    x[3]=xx;
    x[4]=yy;
    updateA();
}

void Binoculus::think()
{
    Ml=0;
    Mr=0;
    if (vehicletype==vehicleTypes::shy_light_seeker){
      float K1=GS.ShySeeker[7];
      float K2=GS.ShySeeker[8];
      Ml=K1/(lefteye.GreenPR[0]+.1)-K2;
      Mr=K1/(righteye.GreenPR[0]+.1)-K2;
    }
    if (vehicletype==vehicleTypes::aggress_light_seeker){
      float K1=GS.AggressSeeker[7];
      Mr=lefteye.GreenPR[0]*K1;
      Ml=righteye.GreenPR[0]*K1;
    }
    if (vehicletype==vehicleTypes::aggress_light_phobe){
      float K1=GS.AggressPhobe[7];
      Ml=lefteye.GreenPR[0]*K1;
      Mr=righteye.GreenPR[0]*K1;
    }
    if (vehicletype==vehicleTypes::shy_light_phobe){
      float K1=GS.ShyPhobe[7];
      float K2=GS.ShyPhobe[8];
      Mr=K1/(lefteye.GreenPR[0]+.1)-K2;
      Ml=K1/(righteye.GreenPR[0]+.1)-K2;
    }
    if (vehicletype==vehicleTypes::predator){
      float K1=GS.Predator[7];
      Mr=lefteye.BluePR[0]*K1;
      Ml=righteye.BluePR[0]*K1;

      if ((abs(getvelocity())<.001)&& abs(getturnrate())<.001)
      {// would be good to do this from the receptors
          Mr=.04;
      }
    }
    if (vehicletype==vehicleTypes::prey){
      float K1=GS.Prey[7];
      float K2=GS.Prey[8];
      Ml=K1/(lefteye.GreenPR[0]+.1)-K2;
      Mr=K1/(righteye.GreenPR[0]+.1)-K2;
    }
    if (vehicletype==vehicleTypes::test_vehicle){
      float K1=GS.TestVehicle[7];
      Mr=lefteye.GreenPR[0]*K1;
      Ml=righteye.GreenPR[0]*K1;
    }
    if (vehicletype==vehicleTypes::speculatrix){
      //doing this in updatespecu
      //Ml=(lefteye.GreenPR[0]);// debugging...
    }
    if (vehicletype==vehicleTypes::bright_prey){
      //MR=(.01/(greenlefteyeresponse+.1)-.03)+(13*redlefteyeresponse);
      //ML=(.01/(greenrighteyeresponse+.1)-.03)+(13*redrighteyeresponse);

      float K1=GS.BrightPrey[7];
      float K2=GS.BrightPrey[8];
      float K3=GS.BrightPrey.at(9);
      //if (rand()<RAND_MAX/8)std::cout << " ";
      //std::cout << K1<<","<<K2<<","<<K3<<std::endl;

      Ml=K1/(lefteye.GreenPR[0]+.1)-K2+(K3*lefteye.RedPR[0]);
      Mr=K1/(righteye.GreenPR[0]+.1)-K2+(K3*righteye.RedPR[0]);
    }
    /*    if (id==999){// April
	Ml=.005;
	Mr=0.001;
	}*/
}

void Binoculus::info()
{
    std::cout << "Name=" << name << "\nid=" << id << std::endl;
    std::cout << "Vehicle type " << vehicletype << "\nnumber of collisions=" << nCollisions
              << "\nHalf width=" << halfwidth << std::endl;

    std::cout << "Colour =";
    if (lcolour==OFF) std::cout << "off " << std::endl;
    else if (lcolour==RED) std::cout << "red " << std::endl;
    else if (lcolour==BLUE) std::cout << "blue " << std::endl;
    else if (lcolour==GREEN) std::cout << "green " << std::endl;
    else if (lcolour==CYAN) std::cout << "cyan " << std::endl;
    else if (lcolour==YELLOW) std::cout << "yellow " << std::endl;
    else std::cout << "(need to update) =" << lcolour << std::endl;

    std::cout << A << std::endl;
}

void Binoculus::setEyeDivergence(float angle,float x,float y)
{
    lefteye.setEyeDiv(angle,x,y);
    righteye.setEyeDiv(-angle,x,-y);
}

/*
  void Binoculus::setSSgains(float K1,float K2){
  BrainGains[0]=K1;
  BrainGains[1]=K2;
  }
*/

void Binoculus::setMIBs(float lM,float lI,float lB,float lBrot)
{
    mass=lM;
    inertia=lI;
    B=lB;
    Brot=Brot;
}

/// Write out the logs for all the vehicles to a matlab/octave file
/// Candidate for a friend function to binoculus, writelog(name,var,lognum)
void writevehiclelogs( std::list<Binoculus>& globalBvl,
                       std::string fname, char var, int lnum )
{
    std::list<Binoculus>::iterator iter;

    std::ofstream sfid(fname.c_str(), ios::out | ios::binary);

    if(GS.logging){
        if (sfid.fail()){
            cout<<"Could not save file "+fname<<endl;
        }else{
            for (iter=globalBvl.begin(); iter != globalBvl.end(); iter++) {
                //-char buffer2[100];
                //-sprintf(buffer2,"V%c%d",var,(*iter).getid());
                std::ostringstream Convert;
                Convert << "V"<<var<<(*iter).getid();
                std::string buffer2= Convert.str();
                //cout << buffer2<<endl;
                if (lnum==2)
                    (*iter).log2.writematlabv4(sfid,buffer2);
                else
                    (*iter).log.writematlabv4(sfid,buffer2);
                cout <<"Writing log "<<lnum<<" for "<<(*iter).name <<endl;
            }
            sfid.close();
        }
    }else{// not logging
        cout << "Please set logging on in bv.ini"<<endl;
    }
} // end writevehiclelogs
