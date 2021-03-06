/***************************************************************************
 *   Copyright (C) 2006 by William Harwin                                  *
 *   shshawin@localhost.localdomain                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef NRWINDOW_H
#define NRWINDOW_H

/**
@author William Harwin
*/

#include <string>
#include <list>
#include "glutMaster.hpp"
#include "glutWindow.hpp"

class Binoculus;

//-----------------------------------------------------------
class NRWindow : public GlutWindow{
public:

  int          height, width;
  int          initPositionX, initPositionY;
    static std::list<Binoculus> globalBvl;
    static std::list<Binoculus>::iterator globalNVselect;

  NRWindow(GlutMaster * glutMaster,
	     int setWidth, int setHeight,
	     int setInitPositionX, int setInitPositionY,
	     char * title);
  ~NRWindow();

    void CallBackDisplayFunc();
    void CallBackReshapeFunc( int w, int h);
    void CallBackIdleFunc();
    void CallBackKeyboardFunc( unsigned char key, int x, int y);
  void CallBackSpecialFunc(unsigned char key, int x, int y);
    void CallBackMouseFunc( int button, int state, int x, int y);

  void StartMoving(GlutMaster * glutMaster);
  void StopMoving(GlutMaster * glutMaster);
private:
    static int s_elapsed_time; // defined in the corresponding .cpp file.
    static int s_loop_time;
    static std::string s_key_buffer;
};


//------------------------------ declare drawing routines -----------------------------------------

void drawRobot( float x, float y, float theta, float eyeangle, int eyeconeang, std::string & title, int flag );
void drawRobot( float x, float y, float theta, float eyeangle, float robotlengthA, float robothalfwidth,
                float eyeposx, int eyeconeang, std::string & title, int flag );
void drawFixedLight( float x, float y );
void drawFloor();
void drawObjectSelector( float x, float y );

void showMessage( GLfloat x, GLfloat y, GLfloat z, std::string & message );

#endif
