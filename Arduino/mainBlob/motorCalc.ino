#include <math.h>

// Adjustable constants
#define mech_heightOffGround 7.0  // Control how high the center of mass is lifted off the ground

// Mechanism measurements
#define mech_lenRod 18.5
#define mech_rInner 5.0
#define mech_numMotors 4
#define mech_rOuter (mech_lenRod - mech_rInner)
#define mech_rCenter (mech_lenRod/2)
#define mech_numFeet (mech_numMotors*2)
#define mech_rCurveFeet 4.0
#define mech_minLiftAngle 25  // Not used, 15 to 20 according to Anthony
#define mech_motorAngle (360.0/mech_numFeet)
#define mech_angleBetween (360.0/mech_numFeet)
#define mech_angleBetween_2 (mech_angleBetween/2)



int16_t motor[4] = {0,0,0,0};
// Create array to enumerate received command
#define STATE_RST 0
#define STATE_FWD 1
#define STATE_BKWD 2
#define STATE_BRAKE 3
#define STATE_CENTERED 4

byte state = STATE_RST;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void MCSetup() {
  
}


// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

float *MCLoop(byte xbcmd,float imuout, bool *lsout) {
//  static int targetAngle = 0;
//  static unsigned long lastTime = millis();
//  unsigned long currentTime = millis();
//  if(currentTime - lastTime >= 500) {
//    if (isHomed()) {
//      targetAngle += 1;
//      if(targetAngle >= 360) {
//        targetAngle -= 360;
//      }
//    }
//    lastTime = currentTime;
//  }
  //Serial.print(targetAngle);



//  float *positions = calcPositions(targetAngle);
//  
//  //
//  
//  return positions;

  static float centeredMotors[4] = {0,0,0,0};
  static float resetMotors[4] = {NAN,NAN,NAN,NAN};
  
  if (!ready || !isHomed()) {
    // if the imu or motor is not ready, still in initializing state
    state = STATE_RST;
  } 
  else if (state == STATE_RST && ready && isHomed()) {
    state = STATE_CENTERED;
  }
  else if (xbcmd == XBEE_FWD) {
    state = STATE_FWD;
  } 
  else if (xbcmd == XBEE_BACK) {
    state = STATE_BKWD;
  } 
  else if (xbcmd == XBEE_STP) {
    state = STATE_BRAKE;
  }
  else if (xbcmd == XBEE_NONE) {
    // no state change
  }
  
  switch (state) {
    case STATE_RST:
      // reset plates
      return resetMotors;
    case STATE_CENTERED:
      // center plates
      return centeredMotors;
    case STATE_FWD:
      // move forward
      return fwd(imuout);
      break;
    case STATE_BKWD:
      // move backward
      return back(imuout);
      break;
    case STATE_BRAKE:
      // stop moving
      return stp(imuout);
      break;
    default:
      // do nothing
      return resetMotors;
      break;
  }

}

float *fwd(float imuout) {
  //return calcPositions();
  Serial.print("Current angle: ");
  Serial.println((int32_t)imuout);
}

float *back(float imuout) {
  Serial.print("Current angle: ");
  Serial.println((int32_t)imuout);
}

float *stp(float imuout) {
  Serial.print("Current angle: ");
  Serial.println((int32_t)imuout);
}

void domainAndOffset(int angle, int *domain, int *offset) {
  //*domain = int(((360-angle+mech_angleBetween/2.0)/(mech_angleBetween)) % mech_numFeet)
  *domain = 0;
  int a = 360-((int)angle)+(int)(mech_angleBetween_2);
  *domain = a/((int)(mech_angleBetween)) % mech_numFeet;
  //*offset = (angle+domain*angleBetween-360) % 360
  *offset = (((int)angle)+*domain*(int)(mech_angleBetween)) % 360;
//  Serial.print(" dom=");
//  Serial.print(*domain);
//  Serial.print(" off=");
//  Serial.print(*offset);
}
float *calcPositionsFromOffset(int offset) {
  float h = mech_heightOffGround;
  float a_0 = h/cos(offset*PI/180);
  float a_1 = h/cos((offset+45)*PI/180);
  float a_nm1 = h/cos((offset-45)*PI/180);//2*mech_rCenter-h/cos((offset-45)*PI/180);
//  Serial.print(" A0=");
//  Serial.print(a_0);
//  Serial.print(" A1=");
//  Serial.print(a_1);
//  Serial.print(" AN1=");
//  Serial.println(a_0);
  static float motors[mech_numMotors];
  for (int i=2; i<mech_numMotors-1; i++) {
    motors[i] = 0;
  }
  motors[0] = (mech_rCenter-a_0);
  motors[1] = (mech_rCenter-a_1);
  motors[mech_numMotors-1] = (a_nm1-mech_rCenter);
  for (int i=0; i<mech_numMotors; i++) {
    if (motors[i] > mech_rOuter-mech_rCenter) {
      motors[i] = mech_rOuter-mech_rCenter;
    }
    if (motors[i] < -(mech_rOuter-mech_rCenter)) {
      motors[i] = -(mech_rOuter-mech_rCenter);
    }
  }
  return motors;
}
float *calcPositions(int angle) {
  int domain, offset;
  domainAndOffset(angle, &domain, &offset);
  float *motorsDomain = calcPositionsFromOffset(offset);
  static float motors[mech_numMotors];
  for (int i=0; i<mech_numMotors; i++) {
    motors[(i+domain)%mech_numMotors] =
      ((i+domain)%mech_numFeet < mech_numMotors) ? motorsDomain[i] : -motorsDomain[i];
  }
  return motors;
}




