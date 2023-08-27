#include <Servo.h>

#define BAUD_RATE 115200

#define NUM_ROWS 5
#define NUM_COLS 6


// Angles to point at top left corner
#define X_TL 67
#define Y_TL 115
// Angles to point at top right corner
#define X_TR 27
#define Y_TR 117

#define X_BL 56
#define Y_BL 86

#define X_BR 20
#define Y_BR 88


#define Y_CENTRE_DISTORTION -6




//
//
//  To Do: Use special character to show you are adding xServo new entry (Or just sense that u r using numbers)
//
//


Servo yServo;
Servo xServo;


#define X 1
#define Y 0

byte pos[2] = { 0, 0 };

void setup() {
  Serial.begin(BAUD_RATE);
	// pinMode(LED_BUILTIN, OUTPUT);
	// digitalWrite(LED_BUILTIN, 0);

  Serial.println(F("Part Finder\n"));
  yServo.attach(9);
  xServo.attach(10);
  pinMode(6, OUTPUT);
}  // setup()

int time_to_receive_input;
int time_to_find_pos;

void loop() {

  while (Serial.available() == 0) {}

  byte buf[3];

  if (Serial.readBytes(buf, 3) == 3){
	pos[0] = buf[1];
	pos[1] = buf[2];

	if (pos[0] >= 1 && pos[0] <= NUM_ROWS &&
		pos[1] >= 1 && pos[1] <= NUM_COLS)
	  	pointLaser();
	else
		error();
  } else
	error();





}  // loop()

void error() {
	for (int i = 0; i < 20; i++) {
		analogWrite(6, 8);
		delay(600);
		analogWrite(6, 0);
		delay(600);
	}
}

void pointLaser() {
  // Interpolate between both sides
  int angle_y_L = map(pos[0], 1, NUM_ROWS, Y_TL, Y_BL);
  int angle_y_R = map(pos[0], 1, NUM_ROWS, Y_TR, Y_BR);
  // int angle_y_L = getAngle(Y, Y_TL, Y_BL);
  // int angle_y_R = getAngle(Y, Y_T, Y_BR);
  int angle_y = (angle_y_L * (NUM_COLS - pos[1]) + angle_y_R * pos[1]) / NUM_COLS;

  int angle_x_T = getAngle(X, X_TL, X_TR);
  int angle_x_B = getAngle(X, X_BL, X_BR);
  int angle_x = (angle_x_T * (NUM_ROWS - pos[0]) + angle_x_B * pos[0]) / NUM_ROWS;

  float y_fraction = float(pos[0] - 1) / (NUM_ROWS - 1);
  angle_x += Y_CENTRE_DISTORTION * (sq(y_fraction) - y_fraction);

  yServo.write(angle_y);
  xServo.write(angle_x);
  delay(100);
  analogWrite(6, 8);

  delay(2000);

  // If there is a new position sent move to it quicker than usual
  unsigned long init_time = millis();
  while (millis() - init_time < 4000) {
	if (Serial.available() >= 3)
		break;
	
	delay(50);
  }

  analogWrite(6, 0);
}


int getAngle(byte axis, double initial_angle, double final_angle) {
  byte num_segments;
  if (axis == 0)  // If on Y-axis
	num_segments = NUM_ROWS;
  else  // If on X-axis
	num_segments = NUM_COLS;

  double p = pos[axis] - 1;

  double tan_theta = tan(radians(initial_angle)) * (1 - p / num_segments)
                     + tan(radians(final_angle)) * (p / num_segments);
  double theta = degrees(atan(tan_theta));
  return byte(theta);
}