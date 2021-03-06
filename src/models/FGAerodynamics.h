/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       FGAerodynamics.h
 Author:       Jon S. Berndt
 Date started: 09/13/00

 ------------- Copyright (C) 1999  Jon S. Berndt (jon@jsbsim.org) -------------

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

HISTORY
--------------------------------------------------------------------------------
09/13/00   JSB   Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGAERODYNAMICS_H
#define FGAERODYNAMICS_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include <string>
#include <vector>
#include <map>

#include "FGModel.h"
#include "math/FGFunction.h"
#include "math/FGColumnVector3.h"
#include "math/FGMatrix33.h"
#include "input_output/FGXMLFileRead.h"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define ID_AERODYNAMICS "$Id: FGAerodynamics.h,v 1.20 2009/11/12 13:08:11 jberndt Exp $"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim {

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** Encapsulates the aerodynamic calculations.
    This class owns and contains the list of force/coefficients that define the
    aerodynamic properties of an aircraft. Here also, such unique phenomena
    as ground effect, aerodynamic reference point shift, and maximum lift curve
    tailoff are handled.

    @code
    <aerodynamics>
       <alphalimits unit="{RAD | DEG}">
         <min> {number} </min>
         <max> {number} </max>
       </alphalimits>
       <hysteresis_limits unit="{RAD | DEG}">
         <min> {number} </min>
         <max> {number} </max>
       </hysteresis_limits>
       <aero_ref_pt_shift_x>  
         <function>
           {function contents}
         </function> 
       </aero_ref_pt_shift_x>  
       <function>
         {function contents}
       </function>
       <axis name="{LIFT | DRAG | SIDE | ROLL | PITCH | YAW}">
         {force coefficient definitions}
       </axis>
       {additional axis definitions}
    </aerodynamics>
    @endcode

    Optionally two other coordinate systems may be used.<br><br>
    1) Body coordinate system:
    @code
       <axis name="{X | Y | Z}">
    @endcode
    <br>
    2) Axial-Normal coordinate system:
    @code
       <axis name="{AXIAL | NORMAL}">
    @endcode
    <br>
    Systems may NOT be combined, or a load error will occur.

    @author Jon S. Berndt, Tony Peden
    @version $Revision: 1.20 $
*/

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class FGAerodynamics : public FGModel, public FGXMLFileRead
{

public:
  /** Constructor
      @param Executive a pointer to the parent executive object */
  FGAerodynamics(FGFDMExec* Executive);
  /// Destructor
  ~FGAerodynamics();

  bool InitModel(void);

  /** Runs the Aerodynamics model; called by the Executive
      @return false if no error */
  bool Run(void);

  /** Loads the Aerodynamics model.
      The Load function for this class expects the XML parser to
      have found the aerodynamics keyword in the configuration file.
      @param element pointer to the current XML element for aerodynamics parameters.
      @return true if successful */
  bool Load(Element* element);

  /** Gets the total aerodynamic force vector.
      @return a force vector reference. */
  FGColumnVector3& GetForces(void) {return vForces;}

  /** Gets the aerodynamic force for an axis.
      @param n Axis index. This could be 0, 1, or 2, or one of the
               axis enums: eX, eY, eZ.
      @return the force acting on an axis */
  double GetForces(int n) const {return vForces(n);}

  /** Gets the total aerodynamic moment vector.
      @return a moment vector reference. */
  FGColumnVector3& GetMoments(void) {return vMoments;}

  /** Gets the aerodynamic moment for an axis.
      @return the moment about a single axis (as described also in the
              similar call to GetForces(int n).*/
  double GetMoments(int n) const {return vMoments(n);}

  /** Retrieves the aerodynamic forces in the wind axes.
      @return a reference to a column vector containing the wind axis forces. */
  FGColumnVector3& GetvFw(void) { return vFw; }

  /** Retrieves the aerodynamic forces in the wind axes, given an axis.
      @param axis the axis to return the force for (eX, eY, eZ).
      @return a reference to a column vector containing the requested wind
      axis force. */
  double GetvFw(int axis) const { return vFw(axis); }

  /** Retrieves the lift over drag ratio */
  inline double GetLoD(void) const { return lod; }

  /** Retrieves the square of the lift coefficient. */
  inline double GetClSquared(void) const { return clsq; }
  inline double GetAlphaCLMax(void) const { return alphaclmax; }
  inline double GetAlphaCLMin(void) const { return alphaclmin; }

  inline double GetHysteresisParm(void) const { return stall_hyst; }
  inline double GetStallWarn(void) const { return impending_stall; }
  double GetAlphaW(void) const { return alphaw; }

  double GetBI2Vel(void) const { return bi2vel; }
  double GetCI2Vel(void) const { return ci2vel; }

  inline void SetAlphaCLMax(double tt) { alphaclmax=tt; }
  inline void SetAlphaCLMin(double tt) { alphaclmin=tt; }

  /** Gets the strings for the current set of coefficients.
      @param delimeter either a tab or comma string depending on output type
      @return a string containing the descriptive names for all coefficients */
  std::string GetCoefficientStrings(const std::string& delimeter) const;

  /** Gets the coefficient values.
      @param delimeter either a tab or comma string depending on output type
      @return a string containing the numeric values for the current set of
      coefficients */
  std::string GetCoefficientValues(const std::string& delimeter) const;

  /** Calculates and returns the wind-to-body axis transformation matrix.
      @return a reference to the wind-to-body transformation matrix.
      */
  FGMatrix33& GetTw2b(void);

  /** Calculates and returns the body-to-wind axis transformation matrix.
      @return a reference to the wind-to-body transformation matrix.
      */
  FGMatrix33& GetTb2w(void);

  std::vector <FGFunction*> * GetCoeff(void) const { return Coeff; }

private:
  enum eAxisType {atNone, atLiftDrag, atAxialNormal, atBodyXYZ} axisType;
  typedef std::map<std::string,int> AxisIndex;
  AxisIndex AxisIdx;
  FGFunction* AeroRPShift;
  typedef vector <FGFunction*> CoeffArray;
  CoeffArray* Coeff;
  FGColumnVector3 vFnative;
  FGColumnVector3 vFw;
  FGColumnVector3 vForces;
  FGColumnVector3 vMoments;
  FGColumnVector3 vDXYZcg;
  FGColumnVector3 vDeltaRP;
  FGMatrix33 mTw2b;
  FGMatrix33 mTb2w;
  double alphaclmax, alphaclmin;
  double alphahystmax, alphahystmin;
  double impending_stall, stall_hyst;
  double bi2vel, ci2vel,alphaw;
  double clsq, lod, qbar_area;

  typedef double (FGAerodynamics::*PMF)(int) const;
  void DetermineAxisSystem(void);
  void bind(void);

  void Debug(int from);
};

} // namespace JSBSim

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#endif

