/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Module:       FGInertial.cpp
 Author:       Jon S. Berndt
 Date started: 09/13/00
 Purpose:      Encapsulates the inertial frame forces (coriolis and centrifugal)

 ------------- Copyright (C) 2000  Jon S. Berndt (jon@jsbsim.org) -------------

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 Place - Suite 330, Boston, MA  02111-1307, USA.

 Further information about the GNU Lesser General Public License can also be found on
 the world wide web at http://www.gnu.org.

FUNCTIONAL DESCRIPTION
--------------------------------------------------------------------------------

HISTORY
--------------------------------------------------------------------------------
09/13/00   JSB   Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "FGInertial.h"
#include "FGFDMExec.h"
#include "FGPropagate.h"
#include "FGMassBalance.h"
#include <iostream>

using namespace std;

namespace JSBSim
{

static const char *IdSrc = "$Id: FGInertial.cpp,v 1.18 2010/03/28 05:57:00 jberndt Exp $";
static const char *IdHdr = ID_INERTIAL;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS IMPLEMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/


FGInertial::FGInertial(FGFDMExec* fgex) : FGModel(fgex)
{
    Name = "FGInertial";

    // Earth defaults
    RotationRate    = 0.00007292115;
    GM              = 14.07644180E15;     // WGS84 value
    RadiusReference = 20925650.00;        // Equatorial radius (WGS84)
    C2_0            = -4.84165371736E-04; // WGS84 value for the C2,0 coefficient
    J2              = 1.0826266836E-03;   // WGS84 value for J2
    a               = 20925646.3255;      // WGS84 semimajor axis length in feet
    b               = 20855486.5951;      // WGS84 semiminor axis length in feet
    earthPosAngle   = 0.0;

    // Lunar defaults
    /*
    RotationRate    = 0.0000026617;
    GM              = 1.7314079E14;         // Lunar GM
    RadiusReference = 5702559.05;           // Equatorial radius
    C2_0            = 0;                    // value for the C2,0 coefficient
    J2              = 2.033542482111609E-4; // value for J2
    a               = 5702559.05;           // semimajor axis length in feet
    b               = 5695439.63;           // semiminor axis length in feet
    earthPosAngle   = 0.0;
    */

    gAccelReference = GM/(RadiusReference*RadiusReference);
    gAccel          = GM/(RadiusReference*RadiusReference);

    bind();

    Debug(0);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

FGInertial::~FGInertial(void)
{
    Debug(1);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool FGInertial::InitModel(void)
{
    if (!FGModel::InitModel()) return false;

    earthPosAngle   = 0.0;

    return true;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool FGInertial::Run(void)
{
    // Fast return if we have nothing to do ...
    if (FGModel::Run()) return true;
    if (FDMExec->Holding()) return false;

    RunPreFunctions();

    // Gravitation accel
    double r = Propagate->GetRadius();
    gAccel = GetGAccel(r);
    earthPosAngle += FDMExec->GetDeltaT()*RotationRate;

    RunPostFunctions();

    return false;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double FGInertial::GetGAccel(double r) const
{
    return GM/(r*r);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// Calculate the WGS84 gravitation value in ECEF frame. Pass in the ECEF position
// via the position parameter. The J2Gravity value returned is in ECEF frame,
// and therefore may need to be expressed (transformed) in another frame,
// depending on how it is used. See Stevens and Lewis eqn. 1.4-16.

FGColumnVector3 FGInertial::GetGravityJ2(FGColumnVector3 position) const
{
    FGColumnVector3 J2Gravity;

    // Gravitation accel
    double r = position.Magnitude();
    double lat = Propagate->GetLatitude();
    double sinLat = sin(lat);

    double preCommon = 1.5*J2*(a/r)*(a/r);
    double xy = 1.0 - 5.0*(sinLat*sinLat);
    double z = 3.0 - 5.0*(sinLat*sinLat);
    double GMOverr2 = GM/(r*r);

    J2Gravity(1) = -GMOverr2 * ((1.0 + (preCommon * xy)) * position(eX)/r);
    J2Gravity(2) = -GMOverr2 * ((1.0 + (preCommon * xy)) * position(eY)/r);
    J2Gravity(3) = -GMOverr2 * ((1.0 + (preCommon *  z)) * position(eZ)/r);

    return J2Gravity;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void FGInertial::bind(void)
{
    PropertyManager->Tie("position/epa-rad", this, &FGInertial::GetEarthPositionAngle);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//    The bitmasked value choices are as follows:
//    unset: In this case (the default) JSBSim would only print
//       out the normally expected messages, essentially echoing
//       the config files as they are read. If the environment
//       variable is not set, debug_lvl is set to 1 internally
//    0: This requests JSBSim not to output any messages
//       whatsoever.
//    1: This value explicity requests the normal JSBSim
//       startup messages
//    2: This value asks for a message to be printed out when
//       a class is instantiated
//    4: When this value is set, a message is displayed when a
//       FGModel object executes its Run() method
//    8: When this value is set, various runtime state variables
//       are printed out periodically
//    16: When set various parameters are sanity checked and
//       a message is printed out when they go out of bounds

void FGInertial::Debug(int from)
{
    if (debug_lvl <= 0) return;

    if (debug_lvl & 1)   // Standard console startup message output
    {
        if (from == 0)   // Constructor
        {

        }
    }
    if (debug_lvl & 2 )   // Instantiation/Destruction notification
    {
        if (from == 0) cout << "Instantiated: FGInertial" << endl;
        if (from == 1) cout << "Destroyed:    FGInertial" << endl;
    }
    if (debug_lvl & 4 )   // Run() method entry print for FGModel-derived objects
    {
    }
    if (debug_lvl & 8 )   // Runtime state variables
    {
    }
    if (debug_lvl & 16)   // Sanity checking
    {
    }
    if (debug_lvl & 64)
    {
        if (from == 0)   // Constructor
        {
            cout << IdSrc << endl;
            cout << IdHdr << endl;
        }
    }
}
}
