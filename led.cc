#include <wiringPi.h>
#include <iostream>
#include <sys/time.h>
#include <softTone.h>
#include <ctime>
#include <softPwm.h>
#include <cmath>

using namespace std;

#define uchar unsigned char
#define LaserPin 0
#define LEDPin   1 
#define BtnPin   3
#define BuzzPin  22
#define TrigPin  7
#define EchoPin  4
#define LedPinRed   24
#define LedPinGreen 26
#define LedPinBlue  27

#define  CL1  131
#define  CL2  147
#define  CL3  165
#define  CL4  175
#define  CL5  196
#define  CL6  221
#define  CL7  248

#define  CM1  262
#define  CM2  294
#define  CM3  330
#define  CM4  350
#define  CM5  393
#define  CM6  441
#define  CM7  495

#define  CH1  525
#define  CH2  589
#define  CH3  661
#define  CH4  700
#define  CH5  786
#define  CH6  882
#define  CH7  990


void ledColorSet(uchar r_val, uchar g_val, uchar b_val)
{
	softPwmWrite(LedPinRed,   r_val);
	softPwmWrite(LedPinGreen, g_val);
	softPwmWrite(LedPinBlue,  b_val);
}


float disMeasure(void)
{
	struct timeval tv1;
	struct timeval tv2;
	long time1, time2;
	float dis;

	digitalWrite(TrigPin, LOW);
	delayMicroseconds(2);

	digitalWrite(TrigPin, HIGH);
	delayMicroseconds(10);     
	digitalWrite(TrigPin, LOW);

	while(!(digitalRead(EchoPin) == 1));
	gettimeofday(&tv1, NULL);        

	while(!(digitalRead(EchoPin) == 0)); 
	gettimeofday(&tv2, NULL);     

	time1 = tv1.tv_sec * 1000000 + tv1.tv_usec;
	time2  = tv2.tv_sec * 1000000 + tv2.tv_usec;

	dis = (float)(time2 - time1) / 1000000 * 34000 / 2;  //求出距离

	return dis * 1 / 2.54 / 12.0;
}

int main() {
	bool led_status = 0;
	if(wiringPiSetup() == -1) {
		cout << "FAILED" << endl;
		return 1;
	}

	if(softToneCreate(BuzzPin) == -1) {
		return 1;
	}

	pinMode(LEDPin, OUTPUT);
	pinMode(BtnPin, INPUT);
	pinMode(EchoPin, INPUT);
	pinMode(TrigPin, OUTPUT);
	pinMode(LaserPin, OUTPUT);
	softPwmCreate(LedPinRed,  0, 100);
	softPwmCreate(LedPinGreen,0, 100);
	softPwmCreate(LedPinBlue, 0, 100);	

	delay(500);
	while(1) {
		digitalWrite(LaserPin, HIGH);
		if(0 == digitalRead(BtnPin)) {
			led_status = !led_status;
			cout << "Button pressed.." << endl;
			if(!led_status) {
				cout << "/tLED OFF" << endl;
				digitalWrite(LEDPin, LOW);
				delay(500);
				cout << "Waiting to turn on the laser." << endl;
				continue;
			}
			srand(time(NULL));
			cout << "\tWe areonna kill" << endl;	
			digitalWrite(LEDPin, HIGH);
			delay(500);
			cout << "\tRANGE SENSOR ON" << endl;
			while(led_status) {
				float distance = 0.0;
				distance = disMeasure();
				if(distance <= .5 || distance >= 7) {
					softToneWrite(BuzzPin, 0);
					continue;
				}

				cout << distance << "ft" << endl;
				delay(100);
				int note = ((int)distance * 200) % 1000;
				softToneWrite(BuzzPin, note);
				digitalWrite(LEDPin, HIGH);
				digitalWrite(LaserPin, LOW);
				delay(50);
				digitalWrite(LaserPin, HIGH);
				delay(50);
				softToneWrite(BuzzPin, 0);

				digitalWrite(LaserPin, LOW);
				delay(50);
				softToneWrite(BuzzPin, note+100);
				digitalWrite(LaserPin, HIGH);
				delay(50);
				softToneWrite(BuzzPin, 0);
				digitalWrite(LEDPin, LOW);
				int r = note;
				int g = note * 82 % 256;
				int b = abs(note - 512) % 256;
				ledColorSet(r,g,b);
				
				if(0 == digitalRead(BtnPin)) {
					led_status = !led_status;
					cout << "BROKE ULTRASONIC RANGE LOOP VIA BUTTON PRESS." << endl;
					delay(300);
					digitalWrite(LEDPin, LOW);
					softToneWrite(BuzzPin, 0);	
				
				}
			}
		}
	}
	return 0;
}
