/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 Header:       FGPropagate.h
 Author:       Jon S. Berndt
 Date started: 1/5/99

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
01/05/99   JSB   Created

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
SENTRY
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef FGPROPAGATE_H
#define FGPROPAGATE_H

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INCLUDES
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#include "models/FGModel.h"
#include "math/FGColumnVector3.h"
#include "math/FGLocation.h"
#include "math/FGQuaternion.h"
#include "math/FGMatrix33.h"
#include <deque>

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
DEFINITIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#define ID_PROPAGATE "$Id: FGPropagate.h,v 1.43 2010/07/25 15:35:11 jberndt Exp $"

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
FORWARD DECLARATIONS
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

namespace JSBSim
{

using std::deque;
class FGInitialCondition;

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DOCUMENTATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** Models the EOM and integration/propagation of state.
    The Equations of Motion (EOM) for JSBSim are integrated to propagate the
    state of the vehicle given the forces and moments that act on it. The
    integration accounts for a rotating Earth.

    The general execution of this model follows this process:

    -Calculate the angular accelerations
    -Calculate the translational accelerations
    -Calculate the angular rate
    -Calculate the translational velocity

    -Integrate accelerations and rates

    Integration of rotational and translation position and rate can be
    customized as needed or frozen by the selection of no integrator. The
    selection of which integrator to use is done through the setting of
    the associated property. There are four properties which can be set:

    @code
    simulation/integrator/rate/rotational
    simulation/integrator/rate/translational
    simulation/integrator/position/rotational
    simulation/integrator/position/translational
    @endcode

    Each of the integrators listed above can be set to one of the following values:

    @code
    0: No integrator (Freeze)
    1: Rectangular Euler
    2: Trapezoidal
    3: Adams Bashforth 2
    4: Adams Bashforth 3
    5: Adams Bashforth 4
    @endcode

    @author Jon S. Berndt, Mathias Froehlich
    @version $Id: FGPropagate.h,v 1.43 2010/07/25 15:35:11 jberndt Exp $
  */

/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
CLASS DECLARATION
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

class FGPropagate : public FGModel
{
public:

    /** The current vehicle state vector structure contains the translational and
      angular position, and the translational and angular velocity. */
    struct VehicleState
    {
        /** Represents the current location of the vehicle in Earth centered Earth
            fixed (ECEF) frame.
            units ft */
        FGLocation vLocation;

        /** The velocity vector of the vehicle with respect to the ECEF frame,
            expressed in the body system.
            units ft/sec */
        FGColumnVector3 vUVW;

        /** The angular velocity vector for the vehicle relative to the ECEF frame,
            expressed in the body frame.
            units rad/sec */
        FGColumnVector3 vPQR;

        /** The angular velocity vector for the vehicle body frame relative to the
            ECI frame, expressed in the body frame.
            units rad/sec */
        FGColumnVector3 vPQRi;

        /** The current orientation of the vehicle, that is, the orientation of the
            body frame relative to the local, NED frame. */
        FGQuaternion qAttitudeLocal;

        /** The current orientation of the vehicle, that is, the orientation of the
            body frame relative to the inertial (ECI) frame. */
        FGQuaternion qAttitudeECI;

        FGColumnVector3 vInertialVelocity;

        FGColumnVector3 vInertialPosition;

        deque <FGColumnVector3> dqPQRdot;
        deque <FGColumnVector3> dqUVWdot;
        deque <FGColumnVector3> dqInertialVelocity;
        deque <FGQuaternion> dqQtrndot;
    };

    /** Constructor.
        The constructor initializes several variables, and sets the initial set
        of integrators to use as follows:
        - integrator, rotational rate = Adams Bashforth 2
        - integrator, translational rate = Adams Bashforth 2
        - integrator, rotational position = Trapezoidal
        - integrator, translational position = Trapezoidal
        @param Executive a pointer to the parent executive object */
    FGPropagate(FGFDMExec* Executive);

    /// Destructor
    ~FGPropagate();

    /// These define the indices use to select the various integrators.
    enum eIntegrateType {eNone = 0, eRectEuler, eTrapezoidal, eAdamsBashforth2, eAdamsBashforth3, eAdamsBashforth4};

    /// These define the indices use to select the gravitation models.
    enum eGravType {gtStandard, gtWGS84};

    /** Initializes the FGPropagate class after instantiation and prior to first execution.
        The base class FGModel::InitModel is called first, initializing pointers to the
        other FGModel objects (and others).  */
    bool InitModel(void);

    /** Runs the Propagate model; called by the Executive.
        @return false if no error */
    bool Run(void);

    const FGQuaternion& GetQuaterniondot(void) const
    {
        return vQtrndot;
    }

    /** Retrieves the velocity vector.
        The vector returned is represented by an FGColumnVector reference. The vector
        for the velocity in Local frame is organized (Vnorth, Veast, Vdown). The vector
        is 1-based, so that the first element can be retrieved using the "()" operator.
        In other words, vVel(1) is Vnorth. Various convenience enumerators are defined
        in FGJSBBase. The relevant enumerators for the vector returned by this call are,
        eNorth=1, eEast=2, eDown=3.
        units ft/sec
        @return The vehicle velocity vector with respect to the Earth centered frame,
                expressed in Local horizontal frame.
    */
    const FGColumnVector3& GetVel(void) const
    {
        return vVel;
    }

    /** Retrieves the body frame vehicle velocity vector.
        The vector returned is represented by an FGColumnVector reference. The vector
        for the velocity in Body frame is organized (Vx, Vy, Vz). The vector
        is 1-based, so that the first element can be retrieved using the "()" operator.
        In other words, vUVW(1) is Vx. Various convenience enumerators are defined
        in FGJSBBase. The relevant enumerators for the vector returned by this call are,
        eX=1, eY=2, eZ=3.
        units ft/sec
        @return The body frame vehicle velocity vector in ft/sec.
    */
    const FGColumnVector3& GetUVW(void) const
    {
        return VState.vUVW;
    }

    /** Retrieves the body axis acceleration.
        Retrieves the computed body axis accelerations based on the
        applied forces and accounting for a rotating body frame.
        The vector returned is represented by an FGColumnVector reference. The vector
        for the acceleration in Body frame is organized (Ax, Ay, Az). The vector
        is 1-based, so that the first element can be retrieved using the "()" operator.
        In other words, vUVWdot(1) is Ax. Various convenience enumerators are defined
        in FGJSBBase. The relevant enumerators for the vector returned by this call are,
        eX=1, eY=2, eZ=3.
        units ft/sec^2
        @return Body axis translational acceleration in ft/sec^2.
    */
    const FGColumnVector3& GetUVWdot(void) const
    {
        return vUVWdot;
    }

    /** Retrieves the body angular rates vector, relative to the ECEF frame.
        Retrieves the body angular rates (p, q, r), which are calculated by integration
        of the angular acceleration.
        The vector returned is represented by an FGColumnVector reference. The vector
        for the angular velocity in Body frame is organized (P, Q, R). The vector
        is 1-based, so that the first element can be retrieved using the "()" operator.
        In other words, vPQR(1) is P. Various convenience enumerators are defined
        in FGJSBBase. The relevant enumerators for the vector returned by this call are,
        eP=1, eQ=2, eR=3.
        units rad/sec
        @return The body frame angular rates in rad/sec.
    */
    const FGColumnVector3& GetPQR(void) const
    {
        return VState.vPQR;
    }

    /** Retrieves the body angular rates vector, relative to the ECI (inertial) frame.
        Retrieves the body angular rates (p, q, r), which are calculated by integration
        of the angular acceleration.
        The vector returned is represented by an FGColumnVector reference. The vector
        for the angular velocity in Body frame is organized (P, Q, R). The vector
        is 1-based, so that the first element can be retrieved using the "()" operator.
        In other words, vPQR(1) is P. Various convenience enumerators are defined
        in FGJSBBase. The relevant enumerators for the vector returned by this call are,
        eP=1, eQ=2, eR=3.
        units rad/sec
        @return The body frame inertial angular rates in rad/sec.
    */
    const FGColumnVector3& GetPQRi(void) const
    {
        return VState.vPQRi;
    }

    /** Retrieves the body axis angular acceleration vector.
        Retrieves the body axis angular acceleration vector in rad/sec^2. The
        angular acceleration vector is determined from the applied forces and
        accounts for a rotating frame.
        The vector returned is represented by an FGColumnVector reference. The vector
        for the angular acceleration in Body frame is organized (Pdot, Qdot, Rdot). The vector
        is 1-based, so that the first element can be retrieved using the "()" operator.
        In other words, vPQRdot(1) is Pdot. Various convenience enumerators are defined
        in FGJSBBase. The relevant enumerators for the vector returned by this call are,
        eP=1, eQ=2, eR=3.
        units rad/sec^2
        @return The angular acceleration vector.
    */
    const FGColumnVector3& GetPQRdot(void) const
    {
        return vPQRdot;
    }

    /** Retrieves the Euler angles that define the vehicle orientation.
        Extracts the Euler angles from the quaternion that stores the orientation
        in the Local frame. The order of rotation used is Yaw-Pitch-Roll. The
        vector returned is represented by an FGColumnVector reference. The vector
        for the Euler angles is organized (Phi, Theta, Psi). The vector
        is 1-based, so that the first element can be retrieved using the "()" operator.
        In other words, the returned vector item with subscript (1) is Phi.
        Various convenience enumerators are defined in FGJSBBase. The relevant
        enumerators for the vector returned by this call are, ePhi=1, eTht=2, ePsi=3.
        units radians
        @return The Euler angle vector, where the first item in the
                vector is the angle about the X axis, the second is the
                angle about the Y axis, and the third item is the angle
                about the Z axis (Phi, Theta, Psi).
    */
    const FGColumnVector3& GetEuler(void) const
    {
        return VState.qAttitudeLocal.GetEuler();
    }

    /** Retrieves a body frame velocity component.
        Retrieves a body frame velocity component. The velocity returned is
        extracted from the vUVW vector (an FGColumnVector). The vector for the
        velocity in Body frame is organized (Vx, Vy, Vz). The vector is 1-based.
        In other words, GetUVW(1) returns Vx. Various convenience enumerators
        are defined in FGJSBBase. The relevant enumerators for the velocity
        returned by this call are, eX=1, eY=2, eZ=3.
        units ft/sec
        @param idx the index of the velocity component desired (1-based).
        @return The body frame velocity component.
    */
    double GetUVW   (int idx) const
    {
        return VState.vUVW(idx);
    }

    /** Retrieves a body frame acceleration component.
        Retrieves a body frame acceleration component. The acceleration returned
        is extracted from the vUVWdot vector (an FGColumnVector). The vector for
        the acceleration in Body frame is organized (Ax, Ay, Az). The vector is
        1-based. In other words, GetUVWdot(1) returns Ax. Various convenience
        enumerators are defined in FGJSBBase. The relevant enumerators for the
        acceleration returned by this call are, eX=1, eY=2, eZ=3.
        units ft/sec^2
        @param idx the index of the acceleration component desired (1-based).
        @return The body frame acceleration component.
    */
    double GetUVWdot(int idx) const
    {
        return vUVWdot(idx);
    }

    /** Retrieves a Local frame velocity component.
        Retrieves a Local frame velocity component. The velocity returned is
        extracted from the vVel vector (an FGColumnVector). The vector for the
        velocity in Local frame is organized (Vnorth, Veast, Vdown). The vector
        is 1-based. In other words, GetVel(1) returns Vnorth. Various convenience
        enumerators are defined in FGJSBBase. The relevant enumerators for the
        velocity returned by this call are, eNorth=1, eEast=2, eDown=3.
        units ft/sec
        @param idx the index of the velocity component desired (1-based).
        @return The body frame velocity component.
    */
    double GetVel(int idx) const
    {
        return vVel(idx);
    }

    /** Retrieves the total inertial velocity in ft/sec.
    */
    double GetInertialVelocityMagnitude(void) const
    {
        return VState.vInertialVelocity.Magnitude();
    }

    /** Retrieves the inertial velocity vector in ft/sec.
    */
    const FGColumnVector3& GetInertialVelocity(void) const
    {
        return VState.vInertialVelocity;
    }

    /** Retrieves the inertial position vector.
    */
    const FGColumnVector3& GetInertialPosition(void) const
    {
        return VState.vInertialPosition;
    }

    /** Returns the current altitude above sea level.
        This function returns the altitude above sea level.
        units ft
        @return The current altitude above sea level in feet.
    */
    double GetAltitudeASL(void)   const
    {
        return VState.vLocation.GetRadius() - SeaLevelRadius;
    }

    /** Returns the current altitude above sea level.
        This function returns the altitude above sea level.
        units meters
        @return The current altitude above sea level in meters.
    */
    double GetAltitudeASLmeters(void) const
    {
        return GetAltitudeASL()*fttom;
    }

    /** Retrieves a body frame angular velocity component relative to the ECEF frame.
        Retrieves a body frame angular velocity component. The angular velocity
        returned is extracted from the vPQR vector (an FGColumnVector). The vector
        for the angular velocity in Body frame is organized (P, Q, R). The vector
        is 1-based. In other words, GetPQR(1) returns P (roll rate). Various
        convenience enumerators are defined in FGJSBBase. The relevant enumerators
        for the angular velocity returned by this call are, eP=1, eQ=2, eR=3.
        units rad/sec
        @param axis the index of the angular velocity component desired (1-based).
        @return The body frame angular velocity component.
    */
    double GetPQR(int axis) const
    {
        return VState.vPQR(axis);
    }

    /** Retrieves a body frame angular velocity component relative to the ECI (inertial) frame.
        Retrieves a body frame angular velocity component. The angular velocity
        returned is extracted from the vPQR vector (an FGColumnVector). The vector
        for the angular velocity in Body frame is organized (P, Q, R). The vector
        is 1-based. In other words, GetPQR(1) returns P (roll rate). Various
        convenience enumerators are defined in FGJSBBase. The relevant enumerators
        for the angular velocity returned by this call are, eP=1, eQ=2, eR=3.
        units rad/sec
        @param axis the index of the angular velocity component desired (1-based).
        @return The body frame angular velocity component.
    */
    double GetPQRi(int axis) const
    {
        return VState.vPQRi(axis);
    }

    /** Retrieves a body frame angular acceleration component.
        Retrieves a body frame angular acceleration component. The angular
        acceleration returned is extracted from the vPQRdot vector (an
        FGColumnVector). The vector for the angular acceleration in Body frame
        is organized (Pdot, Qdot, Rdot). The vector is 1-based. In other words,
        GetPQRdot(1) returns Pdot (roll acceleration). Various convenience
        enumerators are defined in FGJSBBase. The relevant enumerators for the
        angular acceleration returned by this call are, eP=1, eQ=2, eR=3.
        units rad/sec^2
        @param axis the index of the angular acceleration component desired (1-based).
        @return The body frame angular acceleration component.
    */
    double GetPQRdot(int axis) const
    {
        return vPQRdot(axis);
    }

    /** Retrieves a vehicle Euler angle component.
        Retrieves an Euler angle (Phi, Theta, or Psi) from the quaternion that
        stores the vehicle orientation relative to the Local frame. The order of
        rotations used is Yaw-Pitch-Roll. The Euler angle with subscript (1) is
        Phi. Various convenience enumerators are defined in FGJSBBase. The
        relevant enumerators for the Euler angle returned by this call are,
        ePhi=1, eTht=2, ePsi=3 (e.g. GetEuler(eTht) returns Theta).
        units radians
        @return An Euler angle.
    */
    double GetEuler(int axis) const
    {
        return VState.qAttitudeLocal.GetEuler(axis);
    }

    /** Retrieves the cosine of a vehicle Euler angle component.
        Retrieves the cosine of an Euler angle (Phi, Theta, or Psi) from the
        quaternion that stores the vehicle orientation relative to the Local frame.
        The order of rotations used is Yaw-Pitch-Roll. The Euler angle
        with subscript (1) is Phi. Various convenience enumerators are defined in
        FGJSBBase. The relevant enumerators for the Euler angle referred to in this
        call are, ePhi=1, eTht=2, ePsi=3 (e.g. GetCosEuler(eTht) returns cos(theta)).
        units none
        @return The cosine of an Euler angle.
    */
    double GetCosEuler(int idx) const
    {
        return VState.qAttitudeLocal.GetCosEuler(idx);
    }

    /** Retrieves the sine of a vehicle Euler angle component.
        Retrieves the sine of an Euler angle (Phi, Theta, or Psi) from the
        quaternion that stores the vehicle orientation relative to the Local frame.
        The order of rotations used is Yaw-Pitch-Roll. The Euler angle
        with subscript (1) is Phi. Various convenience enumerators are defined in
        FGJSBBase. The relevant enumerators for the Euler angle referred to in this
        call are, ePhi=1, eTht=2, ePsi=3 (e.g. GetSinEuler(eTht) returns sin(theta)).
        units none
        @return The sine of an Euler angle.
    */
    double GetSinEuler(int idx) const
    {
        return VState.qAttitudeLocal.GetSinEuler(idx);
    }

    /** Returns the current altitude rate.
        Returns the current altitude rate (rate of climb).
        units ft/sec
        @return The current rate of change in altitude.
    */
    double Gethdot(void) const
    {
        return -vVel(eDown);
    }

    /** Returns the "constant" LocalTerrainRadius.
        The LocalTerrainRadius parameter is set by the calling application or set to
        sea level + terrain elevation if JSBSim is running in standalone mode.
        units feet
        @return distance of the local terrain from the center of the earth.
        */
    double GetLocalTerrainRadius(void) const;

    double GetSeaLevelRadius(void) const
    {
        return SeaLevelRadius;
    }
    double GetTerrainElevation(void) const;
    double GetDistanceAGL(void)  const;
    double GetRadius(void) const
    {
        if (VState.vLocation.GetRadius() == 0) return 1.0;
        else return VState.vLocation.GetRadius();
    }
    double GetLongitude(void) const
    {
        return VState.vLocation.GetLongitude();
    }
    double GetLatitude(void) const
    {
        return VState.vLocation.GetLatitude();
    }

    double GetGeodLatitudeRad(void) const
    {
        return VState.vLocation.GetGeodLatitudeRad();
    }
    double GetGeodLatitudeDeg(void) const
    {
        return VState.vLocation.GetGeodLatitudeDeg();
    }

    double GetGeodeticAltitude(void) const
    {
        return VState.vLocation.GetGeodAltitude();
    }

    double GetLongitudeDeg(void) const
    {
        return VState.vLocation.GetLongitudeDeg();
    }
    double GetLatitudeDeg(void) const
    {
        return VState.vLocation.GetLatitudeDeg();
    }
    const FGLocation& GetLocation(void) const
    {
        return VState.vLocation;
    }

    /** Retrieves the local-to-body transformation matrix.
        The quaternion class, being the means by which the orientation of the
        vehicle is stored, manages the local-to-body transformation matrix.
        @return a reference to the local-to-body transformation matrix.  */
    const FGMatrix33& GetTl2b(void) const
    {
        return VState.qAttitudeLocal.GetT();
    }

    /** Retrieves the body-to-local transformation matrix.
        The quaternion class, being the means by which the orientation of the
        vehicle is stored, manages the body-to-local transformation matrix.
        @return a reference to the body-to-local matrix.  */
    const FGMatrix33& GetTb2l(void) const
    {
        return VState.qAttitudeLocal.GetTInv();
    }

    /** Retrieves the ECEF-to-body transformation matrix.
        @return a reference to the ECEF-to-body transformation matrix.  */
    const FGMatrix33& GetTec2b(void) const
    {
        return Tec2b;
    }

    /** Retrieves the body-to-ECEF transformation matrix.
        @return a reference to the body-to-ECEF matrix.  */
    const FGMatrix33& GetTb2ec(void) const
    {
        return Tb2ec;
    }

    /** Retrieves the ECI-to-body transformation matrix.
        @return a reference to the ECI-to-body transformation matrix.  */
    const FGMatrix33& GetTi2b(void) const
    {
        return VState.qAttitudeECI.GetT();
    }

    /** Retrieves the body-to-ECI transformation matrix.
        @return a reference to the body-to-ECI matrix.  */
    const FGMatrix33& GetTb2i(void) const
    {
        return VState.qAttitudeECI.GetTInv();
    }

    /** Retrieves the ECEF-to-ECI transformation matrix.
        @return a reference to the ECEF-to-ECI transformation matrix.  */
    const FGMatrix33& GetTec2i(void);

    /** Retrieves the ECI-to-ECEF transformation matrix.
        @return a reference to the ECI-to-ECEF matrix.  */
    const FGMatrix33& GetTi2ec(void);

    /** Retrieves the ECEF-to-local transformation matrix.
        Retrieves the ECEF-to-local transformation matrix. Note that the so-called
        local from is also know as the NED frame (for North, East, Down).
        @return a reference to the ECEF-to-local matrix.  */
    const FGMatrix33& GetTec2l(void) const
    {
        return VState.vLocation.GetTec2l();
    }

    /** Retrieves the local-to-ECEF transformation matrix.
        Retrieves the local-to-ECEF transformation matrix. Note that the so-called
        local from is also know as the NED frame (for North, East, Down).
        @return a reference to the local-to-ECEF matrix.  */
    const FGMatrix33& GetTl2ec(void) const
    {
        return VState.vLocation.GetTl2ec();
    }

    /** Retrieves the local-to-inertial transformation matrix.
        @return a reference to the local-to-inertial transformation matrix.  */
    const FGMatrix33& GetTl2i(void)
    {
        return VState.vLocation.GetTl2i();
    }

    /** Retrieves the inertial-to-local transformation matrix.
        @return a reference to the inertial-to-local matrix.  */
    const FGMatrix33& GetTi2l(void)
    {
        return VState.vLocation.GetTi2l();
    }

    VehicleState* GetVState(void)
    {
        return &VState;
    }

    void SetVState(VehicleState* vstate)
    {
        VState.vLocation = vstate->vLocation;
        VState.vUVW = vstate->vUVW;
        VState.vPQR = vstate->vPQR;
        VState.qAttitudeLocal = vstate->qAttitudeLocal;
        VState.qAttitudeECI = vstate->qAttitudeECI;

        VState.dqPQRdot.resize(4, FGColumnVector3(0.0,0.0,0.0));
        VState.dqUVWdot.resize(4, FGColumnVector3(0.0,0.0,0.0));
        VState.dqInertialVelocity.resize(4, FGColumnVector3(0.0,0.0,0.0));
        VState.dqQtrndot.resize(4, FGColumnVector3(0.0,0.0,0.0));
    }

    void SetInertialOrientation(FGQuaternion Qi);
    void SetInertialVelocity(FGColumnVector3 Vi);

    const FGQuaternion GetQuaternion(void) const
    {
        return VState.qAttitudeLocal;
    }

    void SetPQR(unsigned int i, double val)
    {
        if ((i>=1) && (i<=3) )
            VState.vPQR(i) = val;
    }

    void SetUVW(unsigned int i, double val)
    {
        if ((i>=1) && (i<=3) )
            VState.vUVW(i) = val;
    }

// SET functions

    void SetLongitude(double lon)
    {
        VState.vLocation.SetLongitude(lon);
    }
    void SetLongitudeDeg(double lon)
    {
        SetLongitude(lon*degtorad);
    }
    void SetLatitude(double lat)
    {
        VState.vLocation.SetLatitude(lat);
    }
    void SetLatitudeDeg(double lat)
    {
        SetLatitude(lat*degtorad);
    }
    void SetRadius(double r)
    {
        VState.vLocation.SetRadius(r);
    }
    void SetLocation(const FGLocation& l)
    {
        VState.vLocation = l;
    }
    void SetAltitudeASL(double altASL);
    void SetAltitudeASLmeters(double altASL)
    {
        SetAltitudeASL(altASL/fttom);
    }
    void SetSeaLevelRadius(double tt)
    {
        SeaLevelRadius = tt;
    }
    void SetTerrainElevation(double tt);
    void SetDistanceAGL(double tt);
    void SetInitialState(const FGInitialCondition *);
    void RecomputeLocalTerrainRadius(void);

    void NudgeBodyLocation(FGColumnVector3 deltaLoc)
    {
        vDeltaXYZEC = GetTb2ec()*deltaLoc;
        VState.vLocation -= vDeltaXYZEC;
    }

    struct LagrangeMultiplier
    {
        FGColumnVector3 ForceJacobian;
        FGColumnVector3 MomentJacobian;
        double Min;
        double Max;
        double value;
    };

private:

// state vector

    struct VehicleState VState;

    FGColumnVector3 vVel;
    FGColumnVector3 vPQRdot;
    FGColumnVector3 vUVWdot;
    FGColumnVector3 vInertialVelocity;
    FGColumnVector3 vLocation;
    FGColumnVector3 vDeltaXYZEC;
    FGColumnVector3 vGravAccel;
    FGColumnVector3 vOmegaEarth;  // The Earth angular velocity vector
    FGQuaternion vQtrndot;
    FGMatrix33 Tec2b;
    FGMatrix33 Tb2ec;
    FGMatrix33 Tl2b;   // local to body frame matrix copy for immediate local use
    FGMatrix33 Tb2l;   // body to local frame matrix copy for immediate local use
    FGMatrix33 Tl2ec;  // local to ECEF matrix copy for immediate local use
    FGMatrix33 Tec2l;  // ECEF to local frame matrix copy for immediate local use
    FGMatrix33 Tec2i;  // ECEF to ECI frame matrix copy for immediate local use
    FGMatrix33 Ti2ec;  // ECI to ECEF frame matrix copy for immediate local use
    FGMatrix33 Ti2b;   // ECI to body frame rotation matrix
    FGMatrix33 Tb2i;   // body to ECI frame rotation matrix
    FGMatrix33 Ti2l;
    FGMatrix33 Tl2i;
    FGLocation contactloc;
    FGColumnVector3 dv;

    double LocalTerrainRadius, SeaLevelRadius, VehicleRadius;
    double radInv;
    eIntegrateType integrator_rotational_rate;
    eIntegrateType integrator_translational_rate;
    eIntegrateType integrator_rotational_position;
    eIntegrateType integrator_translational_position;
    int gravType;

    void CalculatePQRdot(void);
    void CalculateQuatdot(void);
    void CalculateInertialVelocity(void);
    void CalculateUVWdot(void);

    void Integrate( FGColumnVector3& Integrand,
                    FGColumnVector3& Val,
                    deque <FGColumnVector3>& ValDot,
                    double dt,
                    eIntegrateType integration_type);

    void Integrate( FGQuaternion& Integrand,
                    FGQuaternion& Val,
                    deque <FGQuaternion>& ValDot,
                    double dt,
                    eIntegrateType integration_type);

    void ResolveFrictionForces(double dt);

    void bind(void);
    void Debug(int from);
};
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include "initialization/FGInitialCondition.h"

#endif
