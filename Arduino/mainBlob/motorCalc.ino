#include <math.h>

// Adjustable constants
#define mech_heightOffGround 8.0  // Control how high the center of mass is lifted off the ground

// Mechanism measurements
#define mech_lenRod 18.5
#define mech_rInner 5.0
#define mech_numMotors 4
#define mech_rOuter (mech_lenRod - mech_rInner)
#define mech_rCenter (mech_lenRod/2)
#define mech_numFeet (mech_numMotors*2)
#define mech_rCurveFeet 4
#define mech_minLiftAngle 25  // Not used, 15 to 20 according to Anthony
#define mech_motorAngle (360.0/mech_numFeet)
#define mech_angleBetween (360.0/mech_numFeet)
#define mech_angleBetween_2 (mech_angleBetween/2)


#define MOTOR_SPEED 6  // Degrees per second
#define START_ANG 0

int16_t motor[4] = {0,0,0,0};
// Create array to enumerate received command
#define STATE_RST 0
#define STATE_FWD 1
#define STATE_BKWD 2
#define STATE_BRAKE 3
#define STATE_CENTERED 4

byte state = STATE_RST;

static float centeredMotors[4] = {0,0,0,0};
static float resetMotors[4] = {NAN,NAN,NAN,NAN};

float storedAngle;
unsigned long lastTime = 0;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void MCSetup() {
  
}


// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

float *MCLoop(byte xbcmd,float imuout, bool ready) {
//  static int targetAngle = 0;
//  static unsigned long lastTime = millis();
//  unsigned long currentTime = millis();
//  if(currentTime - lastTime >= 150) {
//    if (isHomed()) {
//      targetAngle += 1;
//      if(targetAngle >= 360) {
//        targetAngle -= 360;
//      }
//    }
//    lastTime = currentTime;
//  }
//  //PRINT(targetAngle);
//  
//  float *positions = calcPositions(targetAngle);
//  
//  return positions;

  unsigned long currentTime = micros();
  unsigned long timeDiff = currentTime - lastTime;
  lastTime = currentTime;

  static bool storeReady = true;
  if (ready) {
    storeReady = true;
  }
  if (!storeReady || !isHomed()) {
    // if the imu or motor is not ready, still in initializing state
    state = STATE_RST;
    storedAngle = START_ANG;
  } 
  else if (state == STATE_RST && storeReady && isHomed()) {
    state = STATE_CENTERED;
    storedAngle = START_ANG;
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
  else if (xbcmd == XBEE_CENTER) {
    state = STATE_CENTERED;
    storedAngle = START_ANG;
  }
  else if (xbcmd == XBEE_NONE) {
    // no state change
  }
  PRINT("STATE ");
  PRINT(ready ? "ready, " : "not ready, ");
  PRINT(storedAngle);
  PRINT(", ");
  PRINTln(state);

  //return centeredMotors;
  
  switch (state) {
    case STATE_RST:
      // reset plates
      storedAngle = START_ANG;
      return resetMotors;
    case STATE_CENTERED:
      // center plates
      storedAngle = START_ANG;
      return centeredMotors;
    case STATE_FWD:
      // move forward
      storedAngle += timeDiff / 1000000.0 * MOTOR_SPEED;
      if (storedAngle >= 360) storedAngle -= 360;
      return fwd(storedAngle);
      break;
    case STATE_BKWD:
      // move backward
      storedAngle -= timeDiff / 1000000.0 * MOTOR_SPEED;
      if (storedAngle < 0) storedAngle += 360;
      return back(storedAngle);
      break;
    case STATE_BRAKE:
      // stop moving
      // Constant storedAngle
      return stp(storedAngle);
      break;
    default:
      // do nothing
      return resetMotors;
      break;
  }
}

float *fwd(float imuout) {
  if (isnan(imuout)) {
    return resetMotors;
  }
  return calcPositions(imuout);
}

float *back(float imuout) {
  if (isnan(imuout)) {
    return resetMotors;
  }
  return calcPositions(imuout);
}

float *stp(float imuout) {
  if (isnan(imuout)) {
    return resetMotors;
  }
  return calcPositions(imuout);
}

void domainAndOffset(int angle, int *domain, int *offset) {
  //*domain = int(((360-angle+mech_angleBetween/2.0)/(mech_angleBetween)) % mech_numFeet)
  *domain = 0;
  int a = 360-((int)angle)+(int)(mech_angleBetween_2);
  *domain = a/((int)(mech_angleBetween)) % mech_numFeet;
  //*offset = (angle+domain*angleBetween-360) % 360
  *offset = (((int)angle)+*domain*(int)(mech_angleBetween)) % 360;
//  PRINT(" dom=");
//  PRINT(*domain);
//  PRINT(" off=");
//  PRINT(*offset);
}
float *calcPositionsFromOffset(int offset) {
  float h = mech_heightOffGround;
  float t_0 = offset*PI/180;
  float t_1 = (offset+45)*PI/180;
  float t_nm1 = (offset-45)*PI/180;
  float x_0 = mech_rCurveFeet*(1-cos(t_0));
  float x_1 = mech_rCurveFeet*(1-cos(t_1));
  float x_nm1 = mech_rCurveFeet*(1-cos(t_nm1));
  float a_0 = (h-x_0)/cos(t_0);
  float a_1 = (h-x_1)/cos(t_1);
  float a_nm1 = (h-x_nm1)/cos(t_nm1);
//  PRINT(" A0=");
//  PRINT(a_0);
//  PRINT(" A1=");
//  PRINT(a_1);
//  PRINT(" AN1=");
//  PRINTln(a_0);
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
  angle = angle%360;
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




