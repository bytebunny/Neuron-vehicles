/// @todo Get perspective transform sorted out!
/// @todo Fix the camera to one of the vehicles
#include <iostream>
#include <cmath>

#include"config.hpp"
#include "constants.hpp"
#include "vehicle_types.hpp"
#include "nrWindow.h"
#include "bvglobalini.h"
#include "NBV.h"
#include "glutMaster.h"

using std::cout;
using std::endl;
using std::string;

void Set_lights();
void Draw_lights();

GLfloat light0_position[] ={1.0, -0.1, 2.0, 0.0};
GLfloat light1_position[] ={-1.0, 0.0, 2.0, 0.0};

enum {
  NONE, START, STOP, NAMES, NOLIGHT, VOLUME, SHADOW, QUIT, AERIALVIEW,GROUNDVIEW,RELOADINI,SAVELOGS,SHOWTITLES};


void drawRobot(float x,float y,float theta,float eyeangle,float robotlengthA,float robothalfwidth,float eyeposx, int eyeconeang,string & title, int flag);



GlutMaster     * glutMaster;
NRWindow     * Window      = 0;
GLfloat orthratio=2.5;
//GLfloat aspect=1;
GLfloat globalphi = 0;     /* in degrees (-150) */
GLfloat globaltheta = 0;  /* in degrees (30)*/
GLfloat cameraX=0;
GLfloat cameraY=0;
GLfloat cameraZ=10;


extern bool aerialview;
extern int viewheight;
extern int viewwidth;
extern readinifile GS; //global singleton - initialised from a file.
bool showtitles=true;

//-------------------------------------------------

void showMessage( GLfloat x, GLfloat y, GLfloat z, string& message )
{
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glTranslatef(x, y, z);
  glScalef(.003, .003, .003);
  for (int ii=0; ii<message.length(); ++ii){
    glutStrokeCharacter(GLUT_STROKE_ROMAN,message.at(ii));    
  }

  /*  while (*message) {
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *message);
    message++;
    }*/
  glEnable(GL_LIGHTING);
  glPopMatrix();
}


#define MAXWHPTS 100
GLenum initwheelpts=GL_TRUE;
GLfloat Lwheel[MAXWHPTS][3],Rwheel[MAXWHPTS][3],wheelN[MAXWHPTS][3];

static void init_wheel(int n, GLfloat radius, GLfloat width)
{
  int i;
  GLfloat ang;
  //  if (n>=MAXWHPTS){printf("error %d\n",__LINE__);exit(1);}
  if (n>=MAXWHPTS){  cout << "error " << __LINE__ <<endl ;exit(1);}

  for(i=0;i<n;i++){
      ang=(i*2.0*constants::pi)/((GLfloat)n);
    Lwheel[i][0]=radius*sin(ang);
    Lwheel[i][1]=width/2;    
    Lwheel[i][2]=radius*cos(ang);
    Rwheel[i][0]=radius*sin(ang);
    Rwheel[i][1]=-width/2;    
    Rwheel[i][2]=radius*cos(ang);
    wheelN[i][0]=cos(ang);
    wheelN[i][1]=0;    
    wheelN[i][2]=sin(ang);
   }
  initwheelpts=GL_FALSE;
}


/* Create vertices for a wheel with axis along y. */ 
/* Could perhaps use the gluCylinder comand. */
static void drawWheel(int n, GLfloat radius, GLfloat width)
{
  int i,j;

  if (initwheelpts) init_wheel(n,radius,width); // should go in init section eventually
  glBegin(GL_TRIANGLE_FAN);

  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(0.0,width/2,0.0);
  for(i=0;i<n;i++){
    glVertex3fv(&Lwheel[i][0]);
  }
  glVertex3fv(&Lwheel[0][0]);
  glEnd();

  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0.0, -1.0, 0.0);
  glVertex3f(0.0,-width/2,0.0);
  for(i=0;i<n;i++){
    glVertex3fv(&Rwheel[i][0]);
   }
  glVertex3fv(&Rwheel[0][0]);
  glEnd();

  glBegin(GL_QUAD_STRIP);
  for(i=0,j=1;j<n;i++,j++){
    glNormal3fv(&wheelN[i][0]); 
    glVertex3fv(&Lwheel[i][0]);
    glVertex3fv(&Rwheel[i][0]);
    glVertex3fv(&Lwheel[j][0]);
    glVertex3fv(&Rwheel[j][0]);
  }
  glNormal3fv(&wheelN[n-1][0]);
  glVertex3fv(&Lwheel[n-1][0]);
  glVertex3fv(&Rwheel[n-1][0]);
  glVertex3fv(&Lwheel[0][0]);
  glVertex3fv(&Rwheel[0][0]);
  glEnd();
}

GLfloat bright_red_ambient[] = {.8, 0.0, 0.0, 1.0};
GLfloat bright_red_specular[] ={0.9, 0.1, 0.1, 1.0};
GLfloat red_ambient[] = {0.5, 0.0, 0.0, 1.0};
GLfloat red_plastic_diffuse[] ={0.5, 0.0, 0.0,1.0}, 
  red_plastic_specular[] ={0.7, 0.6, 0.6, 1.0}, 
  red_plastic_shininess = 32.0;
GLfloat no_ambient[]={0.0, 0.0, 0.0, 1.0};
GLfloat mat_specular[]={1.0, 1.0, 1.0, 1.0};
GLfloat wheel_ambient[] = {0.70, 0.7, 0.0, 1.0};// wheels are yellow.
GLfloat wheel_specular[] ={0.70, 0.7, 0.0, 1.0};
GLfloat blue_ambient[] = {0.0, 0.0, 0.7, 1.0};
GLfloat blue_specular[] = {0.0, 0.0, 0.7, 1.0};
GLfloat yellow_ambient[] = {1.0, 1.0, 0.0, 1.0};
GLfloat yellow_specular[] ={1.0, 1.0, 0.0, 1.0};
GLfloat bright_green_ambient[] = {0.0, 0.9, 0.0, 1.0};
GLfloat bright_green_specular[] ={0.1, 0.9, 0.1, 1.0};
GLfloat lblue_ambient[] = {0.0, 0.7, 0.7, 1.0};
GLfloat lblue_specular[] ={0.0, 0.7, 0.7, 1.0};
//GLfloat green_ambient[] = {0.0, 0.2, 0.0, 1.0};
GLfloat green_specular[] ={0.00, 0.2, 0.00, 1.0};
/// @todo sort out colours and lights so that a wider range of hues are possible!
/// From KColorChooser
GLfloat gold[]={255,215,0};
GLfloat magenta[]={255,0,255};
GLfloat pink[]={255, 192, 203};
GLfloat sienna[]={160, 82, 45};
GLfloat turquoise[]={64, 224, 208};
GLfloat violet[]={238, 130, 238};
GLfloat wheat[]={245, 222, 179};
GLfloat chocolate[]={210, 105, 30};

/// Draw Object selector (i.e. a sphere to show which vehicle has been selected)
void drawObjectSelector( float x,float y )
{
//  bool haslight=true;
  
  // Draw a box body
  glPushMatrix();
    glTranslatef(x,y,0);
 //   glPushMatrix();                             // Turn on the selector
          glTranslatef(0,0.0,0.1);
	  glMaterialfv(GL_FRONT, GL_SPECULAR, red_plastic_specular);
	  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	  glutSolidSphere(0.1,8,8);
 //   glPopMatrix();      
  glPopMatrix();

}


// Draw Fixed light
void drawFixedLight(float x,float y)
{
  glPushMatrix();
    glTranslatef(x,y,0);
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
      glutSolidCube(0.2);
  glPopMatrix();
}


/// Draw a Mobile robot given state vector - looks like eye angle is
/// in radians and eye cone angle is in degrees!
void
drawRobot(float x,float y,float theta,float eyeangle,int eyeconeang,string & title, int flag)
{
  //float robothalfwidth=.15,robotlength=.5,wheelrad=.1,eyepos_x=0.4;
  float robotlength=.5,robothalfwidth=.15,eyepos_x=0.4;
  drawRobot(x,y,theta,eyeangle,robotlength,robothalfwidth,eyepos_x,eyeconeang,title, flag);

}


void
drawRobot(float x,float y,float theta,float eyeangle, float robotlengthA,float robothalfwidthA, float eyeposA, int eyeconeang,string & title, int flag)
{

  //  GLfloat x, GLfloat y, GLfloat theta;
  bool haslight=false;
  GLfloat robothalfwidth=.15,robotlength=.5,wheelrad=.1,eyepos_x=0.4;

  robotlength=robotlengthA;
  if(robothalfwidth<0.02)
    robothalfwidth=0.02;
  else
    robothalfwidth=robothalfwidthA;
  eyepos_x=eyeposA;


  //Eye cone 
  if (eyeconeang<30)eyeconeang=30;
  if (eyeconeang>130)eyeconeang=130;
  GLfloat eyeconeht=0.2-(eyeconeang-30)*.0015;//0.5 to 0.2
  GLfloat eyeconebase=0.1+(eyeconeang-30)*.001;	//0.1 to 0.2
  
  // Draw a box body
  glPushMatrix();//+1
    glTranslatef(x,y,0);
/*      if(haslight){
        glPushMatrix();                             // has a light
          glTranslatef(0,0.0,0.1);
	  glMaterialfv(GL_FRONT, GL_SPECULAR, red_plastic_specular);
	  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	  glutSolidSphere(0.1,8,8);
        glPopMatrix();
      }*/
//    if(false){
//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
//        glutSolidCube(0.2);
//    }else{

    //if(title!=NULL)showMessage(0.3,0.0, 0.5,title);

    if(showtitles)showMessage(0.3,0.0, 0.5,title);
    glRotatef(theta*180/constants::pi, 0.0, 0.0, 1.0);
      glPushMatrix();                             // Left wheel +2
        glTranslatef(0,robothalfwidth,wheelrad);
	glMaterialfv(GL_FRONT, GL_SPECULAR, wheel_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wheel_ambient);
	drawWheel(16,wheelrad,0.1);
      glPopMatrix();//+1
      glPushMatrix();                             // Right wheel+2
        glTranslatef(0,-robothalfwidth,wheelrad);
	drawWheel(16,wheelrad,0.1);
      glPopMatrix();//+1
      glPushMatrix();                            // Right eye +2
        if (flag!=vehicleTypes::speculatrix){
        glTranslatef(eyepos_x,-robothalfwidth,wheelrad*2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, lblue_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	glutSolidSphere(eyeconebase/2,8,8);
	glRotatef(-90, sin(eyeangle), cos(eyeangle), 0.0);
	  //          glTranslatef(.45,-robothalfwidth,wheelrad*2);
	  //	  glRotatef(-90, 0.4, 0.80, 0.0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue_ambient);
	glutSolidCone(eyeconebase,eyeconeht,5,4);// base .1 height .2
	}
      glPopMatrix();//+1
      glPushMatrix();                            // Left eye +2
        if (flag==vehicleTypes::speculatrix){
          glTranslatef(eyepos_x,0,wheelrad*2);
        }else{
          glTranslatef(eyepos_x,robothalfwidth,wheelrad*2);
        }
	glMaterialfv(GL_FRONT, GL_SPECULAR, lblue_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	glutSolidSphere(eyeconebase/2,8,8);
	glRotatef(-90, -sin(eyeangle), cos(eyeangle), 0.0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue_ambient);
//	glutSolidCone(.1,.2,5,4);
//	glutSolidCylinder(eyeconebase,.1,5,4);
	glutSolidCone(eyeconebase,eyeconeht,5,4);
      glPopMatrix();//+1

      glPushMatrix();                             // Body+2
        glTranslatef(robotlength/3,0.0,0.12);
	glScalef(robotlength,2.0*robothalfwidth,wheelrad+.02);
//	if (robot->thing == vehicleTypes::predator || robot->thing == vehicleTypes::aggress_light_seeker){
	switch (flag){
    case vehicleTypes::fixed_light:
	    //break;
    case vehicleTypes::shy_light_seeker:
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lblue_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, lblue_specular);
	    break;
	  case vehicleTypes::aggress_light_seeker:
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, green_specular);
	    break;
      // case vehicleTypes::aggress_light_phobe:
         // missing
      // break;
	  case vehicleTypes::shy_light_phobe:
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bright_green_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, bright_green_specular);
	    break;
	  case vehicleTypes::prey:
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, yellow_specular);
	    break;
      // case vehicleTypes::bright_prey:
         // missing
      // break;
	  case vehicleTypes::predator:
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bright_red_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, bright_red_specular);
	    break;
	  case vehicleTypes::speculatrix:
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bright_red_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	    break;
	  default: 
	    break;
	
	}//end of else
	glutSolidCube(1);// Body of robot
      glPopMatrix();//+1
  //  }
    glPopMatrix();

}

void drawBoundary(void){/// todo fix the bounary def from brait.ini
  GLfloat boundary=GS.worldboundary;
  GLfloat boundary_white[] = {1.0, 1.0, 0.9, 1.0};//
  glMaterialfv(GL_FRONT, GL_SPECULAR, boundary_white);

  glLineWidth(2.5); 
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINE_STRIP);
  glVertex3f(boundary,boundary, 0.5);
  glVertex3f(-boundary,boundary, 0.5);
  glVertex3f(-boundary,-boundary, 0.5);
  glVertex3f(boundary,-boundary, 0.5);
  glVertex3f(boundary,boundary, 0.5);
  glEnd();
}

void drawFloorTile(GLfloat x,GLfloat y,GLfloat w){
  static GLfloat floorNormal[]={0.0, 0.0, 1.0};
  GLfloat V1[3],V2[3],V3[3],V4[3];
  V1[0]=x+w;V1[1]=y+w;V1[2]=0;
  V2[0]=x-w;V2[1]=y+w;V2[2]=0;
  V3[0]=x-w;V3[1]=y-w;V3[2]=0;
  V4[0]=x+w;V4[1]=y-w;V4[2]=0;
  glBegin(GL_POLYGON);
   glNormal3fv(floorNormal);
   glVertex3fv(V1);
   glVertex3fv(V2);
   glVertex3fv(V3);
   glVertex3fv(V4);
  glEnd();
}

void drawFloor(void){
  //GLfloat tw=6/5;//half the tile width
  GLfloat tw=2;//half the tile width
  GLfloat floor_specular_brown[] = {0.42, .30, 0.06, 1.0};//brown
  GLfloat floor_specular_green[] = {0.1, .30, 0.0, 1.0};//green
  glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular_brown);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);

  for (int ii=-2;ii<=2;ii++){
    for (int jj=-2;jj<=2;jj++){
      if((ii*3+jj)%2){// In fact only need (ii+jj)%2, 3 opens the way for more complex patterns
	glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular_brown);
      }else{
	glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular_green);
      }
      drawFloorTile(ii*2*tw,jj*2*tw,tw);
    }
  }
  drawBoundary();

}


//----------------------------------------------------
void menu( int mode )
{
  cout << "mode " <<mode<<endl;
  if (mode == VOLUME)cout <<"volume selected"<<endl;
  if (mode == QUIT) exit(0);
  if (mode == STOP) Window->StopMoving(glutMaster);   // stop idle function
  if (mode == START) Window->StartMoving(glutMaster);   // stop idle function
  if (mode == AERIALVIEW) aerialview=true;
  if (mode == GROUNDVIEW) aerialview=false;
  if (mode == SHOWTITLES) if(showtitles)showtitles=false;else showtitles=true;
  if (mode == RELOADINI){
    if (GS.ini("bv.ini",0))
      cout << "Warning: could not open bv.ini, using defaults "<<endl;
  }
  if (mode == SAVELOGS){
      writevehiclelogs( NRWindow::globalBvl, "pos1.mat",'p',1);
      writevehiclelogs( NRWindow::globalBvl, "eye2.mat",'e',2);
	if(GS.logging)
	  cout <<"Matlab dump"<<endl;
  }

  //  rendermode = mode;
  //  glutPostRedisplay();
  //if (aerialview) cout << "aerial"<<endl;
  //if (!aerialview) cout << "ground"<<endl;
}

void drawLights(){
  glPushMatrix();
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glTranslatef(light1_position[0],light1_position[1],light1_position[2]);
    glutWireSphere(0.2, 10, 10);
  glPopMatrix();
  glPushMatrix();
    glTranslatef(light0_position[0],light0_position[1],light0_position[2]);
    glutWireSphere(0.2, 10, 10);
  glPopMatrix();
  //  cout <<"Draw lights"<<endl;
}



void
Set_lights(){
  GLfloat white_light[] ={1.0, 1.0, 1.0, 1.0};
  GLfloat grey_light[] ={0.3, 0.4, 0.5, 1.0};

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glShadeModel(GL_SMOOTH);
  //GL_SMOOTH else glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
  glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);

  glLightfv(GL_LIGHT1,GL_POSITION,light1_position);
  glLightfv(GL_LIGHT1,GL_DIFFUSE,grey_light);
  glLightfv(GL_LIGHT1,GL_SPECULAR,grey_light);
}

//--------------------------------------------- class member functions ------------------------------
std::list<Binoculus> NRWindow::globalBvl{};
std::list<Binoculus>::iterator NRWindow::globalNVselect{};
int NRWindow::s_elapsed_time{ 0 }; // define and instantiate the static variables.
int NRWindow::s_loop_time{ 0 };
string NRWindow::s_key_buffer{};

NRWindow::NRWindow(GlutMaster * glutMaster,
                       int setWidth, int setHeight,
                       int setInitPositionX, int setInitPositionY,
                       char * title){

  width  = setWidth;
  height = setHeight;

  initPositionX = setInitPositionX;
  initPositionY = setInitPositionY;
  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(initPositionX, initPositionY);

#if not defined(__APPLE__)
  glViewport(0, 0, width, height);   // This may have to be moved to after the next line on some platforms
#endif
  glutMaster->CallGlutCreateWindow(title, this);
#if defined(__APPLE__)
  glViewport(0, 0, width, height);
#endif

   //   Set_lights();
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  GLdouble aspect=(1.0*viewwidth)/viewheight;
  glLoadIdentity();
  gluPerspective(45.0, aspect, 0.001, 100.0); // perspective transformation (changing this will require changing the 830 constant used in 's' select)

  //  glOrtho(-80.0, 80.0, -80.0, 80.0, -500.0, 500.0);
  //glOrtho(-8.0, 8.0, -8.0, 8.0, -100.0, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  Set_lights();
   //   Draw_lights();
   //   glRotatef(60, 1, 1, 1);

  glutCreateMenu( menu );

  // glutAddMenuEntry("No Shadows", NONE);
  glutAddMenuEntry("Start world", START);
  glutAddMenuEntry("Stop world", STOP);
  glutAddMenuEntry("Aerial view", AERIALVIEW);
  glutAddMenuEntry("Ground view", GROUNDVIEW);
  glutAddMenuEntry("Toggle titles", SHOWTITLES);
  glutAddMenuEntry("Reload ini file", RELOADINI);
  glutAddMenuEntry("Save log files", SAVELOGS);
  // glutAddMenuEntry("Toggle names", NAMES);
  // glutAddMenuEntry("No Light", NOLIGHT);
  // glutAddMenuEntry("Show Volume", VOLUME);
  // glutAddMenuEntry("Shadows", SHADOW);
  glutAddMenuEntry("Quit", QUIT);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

NRWindow::~NRWindow(){

   glutDestroyWindow(windowID);
}




void NRWindow::CallBackReshapeFunc( int w, int h)
{
  viewheight=h;
  viewwidth=w;

  glMatrixMode(GL_PROJECTION); // projection matrix is active
  GLdouble aspect=(1.0*viewwidth)/viewheight;
  //glLoadIdentity();
  //gluPerspective(45.0, aspect, 0.001, 100.0); // perspective transformation
  glMatrixMode(GL_MODELVIEW); // return to modelview mode

  cout << "Reshape called" <<endl;
  // oct14 - need to clean up the following.
  //float ratio =  ((float) w) / ((float) h); // window aspect ratio
  //glLoadIdentity(); // reset the projection
  //gluPerspective(45.0, ratio, 0.1, 100.0); // perspective transformation
  //  glMatrixMode(GL_MODELVIEW); // return to modelview mode
  //glViewport(0, 0, w, h); // set viewport (drawing area) to entire window


  int val;
  if (w<h)val=w;else val=h;

  width = val;
  height= val;
//  glViewport(0, 0, (GLsizei) width, (GLsizei) height);
//  glMatrixMode(GL_MODELVIEW); // return to modelview mode
  //  glViewport(0, 0, width, height); 
  CallBackDisplayFunc();
}



//--------------------------------------------------

void makealight(){
    ( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::fixed_light;
    ( *(NRWindow::globalNVselect) ).name="L";
    ( *(NRWindow::globalNVselect) ).setcolour(GREEN);
}

void makeashyseeker(){
    ( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::shy_light_seeker;
    ( *(NRWindow::globalNVselect) ).name="Shy Seeker";
    ( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.ShySeeker[4]*constants::pi/180,0.4,0.1);
    ( *(NRWindow::globalNVselect) ).setMIBs(GS.ShySeeker[0],GS.ShySeeker[1],GS.ShySeeker[2],GS.ShySeeker[3]);// set the mass, inertia and damping
    //( *(NRWindow::globalNVselect) ).setSSgains(GS.ShySeeker[5],GS.ShySeeker[6]);
    ( *(NRWindow::globalNVselect) ).setcolour(BLUE);

}

void makeanagressiveseeker(){
    ( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::aggress_light_seeker;
    ( *(NRWindow::globalNVselect) ).name="Aggressive Seeker";
    ( *(NRWindow::globalNVselect) ).setEyeDivergence(.45,0,0);
    ( *(NRWindow::globalNVselect) ).setcolour(BLUE);
}


//////////////////////////////////////////////////////////////////////// start of cut-out piece from brait032.cpp
void NRWindow::StartMoving(GlutMaster * glutMaster){

   glutMaster->SetIdleToCurrentWindow();
   glutMaster->EnableIdleFunction();
}

void NRWindow::StopMoving(GlutMaster * glutMaster){

   glutMaster->SetIdleToCurrentWindow();
   glutMaster->DisableIdleFunction();
}

// Can't get this to work...
void NRWindow::CallBackSpecialFunc(unsigned char key, int x, int y)
{
  switch (key) {
  case GLUT_KEY_UP : cout <<"Glut key up"<<endl; break;
  case GLUT_KEY_DOWN : cout <<"Glut key down"<<endl; break;
  default:
    cout << "Special character  " << key << " at "<< x <<", "<<y<<endl;
    Window->StopMoving(glutMaster);   // stop idle function
    break;
  }
  cout << "Special currently not getting called..."<<endl;
}

void NRWindow::CallBackMouseFunc( int button, int state, int mx, int my)
{

  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      cout << "down  " << button << " is at "<< state <<endl;
    }
    if (state == GLUT_UP) {
      cout << "up  " << button << " is at "<< state <<"x="<<mx<<"y="<<my<<endl;

      // direct copy from 's'

    float yy=(viewheight/2-my)*cameraZ/830.0+cameraY;
    float xx=(mx-viewwidth/2)*cameraZ/830.0+cameraX;
    std::list<Binoculus>::iterator iter;

    float nearest=9999.0;
    float delx,dely,d2;
    for (iter=NRWindow::globalBvl.begin(); iter != NRWindow::globalBvl.end(); iter++){
      delx=(*iter).getx()-xx;
      dely=(*iter).gety()-yy;
      d2=delx*delx+dely*dely;
      if (d2<nearest){NRWindow::globalNVselect=iter;nearest=d2;}
    }



    }
  }
}


void NRWindow::CallBackKeyboardFunc( unsigned char key, int mx, int my)
{
  switch (key) {
  case 'a':/// Add a light
    {
      float yy=(viewheight/2-my)*cameraZ/830.0+cameraY;
      float xx=(mx-viewwidth/2)*cameraZ/830.0+cameraX;
      cout <<"xx="<<xx<<",yy="<<yy<<endl;
      if (xx>GS.worldboundary)xx=GS.worldboundary;
      if (xx<-GS.worldboundary)xx=-GS.worldboundary;
      if (yy>GS.worldboundary)yy=GS.worldboundary;
      if (yy<-GS.worldboundary)yy=-GS.worldboundary;
      Binoculus newvehicle("A",-3* constants::pi /4,xx,yy,GREEN);
      //newbeast.change( vehicleTypes::fixed_light );
      //newbeast.settheta();
      NRWindow::globalBvl.push_back (newvehicle);
      NRWindow::globalNVselect=NRWindow::globalBvl.end();
      NRWindow::globalNVselect--;

      //NRWindow::globalNVselect=NRWindow::globalBvl.end()--;
    }
    break;
  case 'b':// not sure what this is for...
    aerialview=true;
    GLfloat gx,gy;
    gx=( *(NRWindow::globalNVselect) ).getx();
    gy=( *(NRWindow::globalNVselect) ).gety();
    break;
  case 'e':/// Erase/delete NCV
      if (NRWindow::globalBvl.size()>1){
        NRWindow::globalBvl.erase(NRWindow::globalNVselect);
        NRWindow::globalNVselect=NRWindow::globalBvl.begin();
      }
      break;
  case 'f':/// rotate world  counter clockwise
      globalphi +=1;
      break;
  case 'g':/// rotate world clockwise
      globalphi +=-1;
      break;
  case 'h':/// help
    GLdouble pmatrix[16],mvmatrix[16];
    //int i;
    cout << "  Braitenberg Vehicle Simulator\n  -----------------------------\n";
    cout << "Version "<< config::project_version << endl;
    glGetDoublev(GL_PROJECTION_MATRIX,pmatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX,mvmatrix);
    cout << "Projection Matrix"<< endl;
    for(int i=0;i<4;i++)
      cout << pmatrix[i]<<"\t"<<pmatrix[i+4]<<"\t"<<pmatrix[i+8]<<"\t"<<pmatrix[i+12]<<endl;
    cout <<endl;
    cout <<"ModelView Matrix"<<endl;
    for(int i=0;i<4;i++)
      cout << mvmatrix[i]<<"\t"<<mvmatrix[i+4]<<"\t"<<mvmatrix[i+8]<<"\t"<<mvmatrix[i+12]<<endl;
    cout <<endl;
    //cout << "x="<<x<<",y="<<y<<endl;
    break;
  case 'i':/// Information on selected vehicle
    ( *(NRWindow::globalNVselect) ).info();
    cout << "loop time (mS) ="<<NRWindow::s_loop_time<<endl;
    cout << "elapsed time (mS) ="<<NRWindow::s_elapsed_time<<endl;
    break;

  case 'l':/// Reload bv.ini
    if (GS.ini("bv.ini",0))
      cout << "Warning: could not open bv.ini, using defaults "<<endl;
    GS.inspect();
    break;

  case 'm':/// Move a light or beast
      {
	float yy=(viewheight/2-my)*cameraZ/830.0+cameraY;
	float xx=(mx-viewwidth/2)*cameraZ/830.0+cameraX;
	( *(NRWindow::globalNVselect) ).relocate(xx,yy);
      }
      break;

  case 'n':/// Select next vehicle
      if (NRWindow::globalBvl.size()>1){
        NRWindow::globalNVselect++;
        if (NRWindow::globalNVselect==NRWindow::globalBvl.end())NRWindow::globalNVselect=NRWindow::globalBvl.begin();
      }
      break;
  case 'p':/// Select previous vehicle
      if (NRWindow::globalBvl.size()>1){
        if (NRWindow::globalNVselect==NRWindow::globalBvl.begin())NRWindow::globalNVselect=NRWindow::globalBvl.end();
        NRWindow::globalNVselect--;
      }
      break;

  case 'r':/// Reset the world
      globalphi = globaltheta=0;
      cameraX=cameraY=0;
      break;
  case 's':/// Select the nearest NCV
    {
    float yy=(viewheight/2-my)*cameraZ/830.0+cameraY;
    float xx=(mx-viewwidth/2)*cameraZ/830.0+cameraX;
    std::list<Binoculus>::iterator iter;

    float nearest=9999.0;
    float delx,dely,d2;
    for (iter=NRWindow::globalBvl.begin(); iter != NRWindow::globalBvl.end(); iter++){
      delx=(*iter).getx()-xx;
      dely=(*iter).gety()-yy;
      d2=delx*delx+dely*dely;
      if (d2<nearest){NRWindow::globalNVselect=iter;nearest=d2;}
    }
  }
    break;


  case 'q':/// quit
  case 27:///  quit
    exit(1);
    break;
  case 'o':/// currently identical to 'z'
    cameraZ=cameraZ+1.5;
    //    orthratio=orthratio*1.260;
    //changeview();
    break;
  case 'O':/// currently identical to 'Z'
    cameraZ=cameraZ-1.5;
    //orthratio=orthratio/1.260;
    //changeview();
    break;
  case 't':/// rotate towards
    globaltheta +=1;
    break;

  case 'v':/// rotate away
      globaltheta +=-1;
      break;

  case 'x':/// move camera +ve x
    cameraX=cameraX+.5;
    break;
  case 'X':/// move camera -ve x
    cameraX=cameraX-.5;
    break;
  case 'Y':/// move camera -ve y
    cameraY=cameraY-.5;
    break;
  case 'y':/// move camera +ve y
    cameraY=cameraY+.5;
    break;
  case 'Z':/// Camera out
    cameraZ=cameraZ-2;
    if (cameraZ<0)
      cameraZ=1.5;
    //CallBackDisplayFunc();
//    changeview();
    break;
  case 'z':/// Camera in
    cameraZ=cameraZ+2;
    //CallBackDisplayFunc();
    //changeview();
    break;
  case ' ':/// start simulation
    cout << "setting idle to moving" <<endl;
    Window->StartMoving(glutMaster);   // enable idle function
    break;
  case '>':
    if (( *(NRWindow::globalNVselect) ).vehicletype <7)( *(NRWindow::globalNVselect) ).vehicletype++;
    //      (*ncvselect).incncv();
      break;
  case '<':
    if (( *(NRWindow::globalNVselect) ).vehicletype >0)( *(NRWindow::globalNVselect) ).vehicletype--;
    //(*ncvselect).decncv();
      break;
    break;
  case 'A':/// Two letter command
  case 'B':/// Two letter command
  case 'D':/// Two letter command
  case 'L':/// Save the log files
  case 'M':/// Two letter command
  case 'P':/// Two letter command
  case 'R':/// Two letter command
  case 'S':/// Two letter command
  case 'T':/// Two letter command
  case 'U':/// Two letter command
  case 'V':/// Two letter command
  case '1':/// Two letter command
  case '2':/// Two letter command
  case '3':/// Two letter command
  case '4':/// Two letter command
    {
      //NRWindow::s_key_buffer.append(""+key);
      NRWindow::s_key_buffer +=key;
      //cout << key<<"->"<<NRWindow::s_key_buffer<<endl;
      cout << "->"<<NRWindow::s_key_buffer<<endl;
      if(NRWindow::s_key_buffer.find("RL")!=std::string::npos){ // Reset to a light
	( *(NRWindow::globalNVselect) ).resetstates();
	makealight();
	NRWindow::s_key_buffer.clear();
      }
      if(NRWindow::s_key_buffer.find("DUMP")!=std::string::npos){ // Dump a matlab file
          writevehiclelogs( NRWindow::globalBvl, "pos1.mat",'p',1);
          writevehiclelogs( NRWindow::globalBvl, "eye2.mat",'e',2);
	cout <<"Matlab dump"<<endl;
	NRWindow::s_key_buffer.clear();
      }
      if(NRWindow::s_key_buffer.find("AP")!=std::string::npos){// Aggressive Phobe
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::aggress_light_phobe;
	( *(NRWindow::globalNVselect) ).name="Aggressive Phobe";
	( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.AggressPhobe.at(6)*constants::pi/180,0,0);
	( *(NRWindow::globalNVselect) ).setcolour(BLUE);
	cout <<"found AP"<<endl;
	NRWindow::s_key_buffer.clear();
      }
      if(NRWindow::s_key_buffer.find("SS")!=std::string::npos){// Shy Seeker
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::shy_light_seeker;
	( *(NRWindow::globalNVselect) ).name="Shy Seeker";
	( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.ShySeeker.at(6)*constants::pi/180,.5,.15);
	( *(NRWindow::globalNVselect) ).setcolour(BLUE);
	//makeashyseeker();
	cout <<"found SS"<<endl;
	NRWindow::s_key_buffer.clear();
      }
      if(NRWindow::s_key_buffer.find("SP")!=std::string::npos){// Shy Phobe
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::shy_light_phobe;
	( *(NRWindow::globalNVselect) ).name="Shy Phobe";
	( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.ShyPhobe.at(6)*constants::pi/180,0,0);
	( *(NRWindow::globalNVselect) ).setcolour(BLUE);
	cout <<"found SP"<<endl;
	NRWindow::s_key_buffer.clear();
      }
      if(NRWindow::s_key_buffer.find("AS")!=std::string::npos){// Aggressive Seeker
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::aggress_light_seeker;
	( *(NRWindow::globalNVselect) ).name="Aggressive Seeker";
	( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.AggressSeeker.at(6)*constants::pi/180,0,0);
	( *(NRWindow::globalNVselect) ).setcolour(RED);
	cout <<"found DP"<<endl;
	NRWindow::s_key_buffer.clear();
      }

      if(NRWindow::s_key_buffer.find("DD")!=std::string::npos){// Agressive Seeker (DD)
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::aggress_light_seeker;
	makeanagressiveseeker();
	cout <<"found AS"<<endl;
	NRWindow::s_key_buffer.clear();
      }





      if(NRWindow::s_key_buffer.find("DP")!=std::string::npos){// Dumb Prey
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::prey;
	( *(NRWindow::globalNVselect) ).name="Dumb Prey";
	( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.Prey.at(6)*constants::pi/180,0.5,0.15);
	( *(NRWindow::globalNVselect) ).setcolour(BLUE);
	cout <<"found DP"<<endl;
	NRWindow::s_key_buffer.clear();
      }
      if(NRWindow::s_key_buffer.find("BP")!=std::string::npos){// Bright Prey
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::bright_prey;
	( *(NRWindow::globalNVselect) ).name="Bright Prey";
	( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.BrightPrey.at(6)*constants::pi/180,0.5,0.15);
	( *(NRWindow::globalNVselect) ).setcolour(BLUE);
	cout <<"found BP"<<endl;
	NRWindow::s_key_buffer.clear();
      }
      if(NRWindow::s_key_buffer.find("PR")!=std::string::npos){// Predator
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::predator;
	( *(NRWindow::globalNVselect) ).name="Predator";
	( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.Predator.at(6)*constants::pi/180,0,0);
	( *(NRWindow::globalNVselect) ).setcolour(RED);
	cout <<"found PR"<<endl;
	NRWindow::s_key_buffer.clear();
      }
      if(NRWindow::s_key_buffer.find("MS")!=std::string::npos){// Test Vehicle
	cout <<"Machina Specularix\n";
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::speculatrix;
	( *(NRWindow::globalNVselect) ).name="M.Speculatrix";
	( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.Predator.at(6)*constants::pi/180,0,0);
	cout << "eye field set to "<<GS.Predator.at(6)<<"degrees"<<endl;
	//( *(NRWindow::globalNVselect) ).setcolour(RED);
	( *(NRWindow::globalNVselect) ).setcolour(GREEN);
	( *(NRWindow::globalNVselect) ).lefteye.inituniform(1,60*constants::pi/180);

	NRWindow::s_key_buffer.clear();
      }
      if(NRWindow::s_key_buffer.find("TV")!=std::string::npos){// Test Vehicle
	( *(NRWindow::globalNVselect) ).resetstates();
	( *(NRWindow::globalNVselect) ).vehicletype=vehicleTypes::test_vehicle;
	( *(NRWindow::globalNVselect) ).name="Test Vehicle";
	( *(NRWindow::globalNVselect) ).setEyeDivergence(GS.TestVehicle.at(6)*constants::pi/180,0,0);
	( *(NRWindow::globalNVselect) ).setcolour(BLUE);
	cout <<"found TV"<<endl;
	NRWindow::s_key_buffer.clear();
      }

    }
    break;
  default:
    cout << "character  " << key << " at "<< mx <<", "<<my<<endl;
    Window->StopMoving(glutMaster);   // stop idle function
    break;
  }
  glutPostRedisplay();// should not need to do this, but if missing then it does not refresh.
  CallBackDisplayFunc();
}


void NRWindow::CallBackDisplayFunc(){
  std::list<Binoculus>::iterator iter;
  float xx,yy,th;
  //int eyecone;
  //float eyeangle;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();/// @todo should be able to reset the proj or the other matrix so as to avoid this push/pop!


   glRotatef(globaltheta, 1.0, 0.0, 0.0);
   glRotatef(globalphi, 0.0, 0.0, 1.0);

//   Draw_lights();

/// glutSolidTeapot(10);
/// Teapot coordinate frame, x-axis out of spout , y-axis out of lid, z-axis to rightl


//   drawRobot(1.5,1.0,1.0,90.0*constants::pi/180.0,90,"test",1);
//   drawRobot(0,-2,0,0,230,NULL,3);
   drawFloor();
   //   drawLights();
//   drawObjectSelector(0,0);


   for (iter=NRWindow::globalBvl.begin(); iter != NRWindow::globalBvl.end(); iter++) {
     xx=(*iter).getx();
     yy=(*iter).gety();
     th=(*iter).getth();

     if((*iter).vehicletype==vehicleTypes::fixed_light){
       drawFixedLight(xx,yy);
     }else{
       float eyediv=(*iter).getEyeDiv();
       // should save this info in eyes... (*iter).getRetinaAngle()*180/constants::pi; For now=60degrees

       // this version uses vehicle dimensions .5,.3.6,drawRobot(xx,yy,th,eyediv,.5,.3,.6,60,(*iter).name,(*iter).vehicletype);

       drawRobot(xx,yy,th,eyediv,60,(*iter).name,(*iter).vehicletype);
     }

     /*     if((*iter).btype()==vehicleTypes::fixed_light)

     else{
       eyeangle=(*iter).eyeth();
       eyecone=(*iter).eye_cone();

       drawRobot(xx,yy,th,eyeangle,eyecone,(*iter).title(),(*iter).btype());
       }*/
//      drawObjectSelector(xx,yy);
   }//end:for

  xx=( *(NRWindow::globalNVselect) ).getx();
  yy=( *(NRWindow::globalNVselect) ).gety();
  drawObjectSelector(xx,yy);

  /* attempt to get labels to orient and draw right.
  for (iter=bovalist.begin(); iter != bovalist.end(); iter++){
    if((*iter).btype()!=vehicleTypes::fixed_light){
      xx=(*iter).x();
      yy=(*iter).y();
      showMessage(xx,yy,0,(*iter).title());
    }
    }*/

  glPopMatrix();

  glLoadIdentity();
  // Option to put the camera on the selected robot
  if (aerialview){
    glTranslatef(-cameraX,-cameraY,-cameraZ);
  }else{// needs work, is tracking the vehicle but not the orientation...
    // could have three options and use also the glTranslatef here with xx/yy
    GLdouble s= sin(( *(NRWindow::globalNVselect) ).getth());
    GLdouble c= cos(( *(NRWindow::globalNVselect) ).getth());
    //cout << ( *(NRWindow::globalNVselect) ).getth()<<" c="<<c<<"s="<<s<<endl;
    gluLookAt(xx-2.5*c,yy-2.5*s,cameraZ/2.0,
	      xx,yy,0.5,
	      0,0,1);
  }
  glFlush();
  glutSwapBuffers();
}//end: CallBackDisplayFunc

/*!
Routine to cycle through vehicles and see what lights they see
This is the Glutmaster idle function
*/
void NRWindow::CallBackIdleFunc(){
  std::list<Binoculus>::iterator iter,lightiter;

    for (iter=NRWindow::globalBvl.begin(); iter != NRWindow::globalBvl.end(); iter++) {
      if ((*iter).vehicletype!=vehicleTypes::fixed_light){
      (*iter).look();
      //if (GS.brainversion==1)(*iter).think();
      (*iter).think();
      if ((*iter).vehicletype == vehicleTypes::speculatrix){
	(*iter).updatespecu();
	//(*iter).lefteye.clearRetina();// probably pointless, but trying to track down a bug.
      }else{
	(*iter).updatestate();
      }
      if(GS.logging)(*iter).log.addcolumn((*iter).getth(),(*iter).getx(),(*iter).gety()); // theta x y
      }
    }
    for (iter=NRWindow::globalBvl.begin(); iter != NRWindow::globalBvl.end(); iter++) {
      if ((*iter).vehicletype!=vehicleTypes::fixed_light){
      eulerstep(* iter);// don't euler step lights
      worldphysics(* iter);
      }
    }
    //}
    {
      int previoustime=NRWindow::s_elapsed_time;
      NRWindow::s_elapsed_time = glutGet(GLUT_ELAPSED_TIME);
      NRWindow::s_loop_time=NRWindow::s_elapsed_time-previoustime;
    }
    CallBackDisplayFunc();
}//end: callbackidlefunction
//////////////////////////////////////////////////////////////////////// end of cut-out piece from brait032.cpp
