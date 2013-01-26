#include "WPILib.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	RobotDrive myRobot; // robot drive system
	Joystick rightstick ; // Right joystick
	Joystick leftstick; //Left Stick
	Jaguar *motor1;
	Jaguar *motor2;	
	Jaguar *motor3;
	DigitalInput *switch1;
	DigitalInput *switch2;
	DigitalInput *sensor1;
	Encoder *encoder1;
	Compressor *c;
	
	
#define NUM_S 2
	Solenoid *s[NUM_S];
	
	float currentDir;
	
#define SECONDS_JAM_SENSOR 3

	float currentJamTimer;
	bool checkJam;
	
	time_t begin, end;
	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		leftstick(1), // These must be declared above
		rightstick(2) //tank drive
		
	{
		myRobot.SetExpiration(0.1);
		
		//Register 3 extra motors (in addition to the two drive motors)
		motor1 = new Jaguar(3); //PWM Channel 3
		motor2 = new Jaguar(4); //PWM Channel 4
		motor3 = new Jaguar(5); //PWM Channel 5
		
		//Two Bumber switches
		switch1 = new DigitalInput(1); //Digital input port 1
		switch2 = new DigitalInput(2); //Digital input port 2
		
		//IR Sensor, Digital input port 4
		sensor1 = new DigitalInput(4);
		
		//Encoder
		encoder1 = new Encoder(5, 6, true); //A channel - Digital Input 5, B Channel - Digital Input 6
		encoder1->Start(); //Start counting
		
		//Compressor
		c = new Compressor(3, 1); //Pressure switch connected to Digital Input 3
		//Spike relay conencted to port 1
		c->Start();
		
		//Call constructor for solenoid objects
		for(int i = 0; i < NUM_S; ++i)
		{
			s[i] = new Solenoid(i + 1);
			s[i]->Set(false);
		}
		
		//Initialize the current direction value, used for the bumper switch logic
		currentDir = 1;
		
		//Set up jam timer, used for counting down
		currentJamTimer = SECONDS_JAM_SENSOR;
		checkJam = false;
		
		//Initialized smart dashboard
		SmartDashboard::init();
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(-0.5, 0.0); 	// drive forwards half speed
		Wait(2.000);		
			
		
		myRobot.Drive(0.0, 0.0); 	// stop robot
		
		
	}

	/**
	 * Runs the motors with Tank steering. 
	 */
	
	
	void OperatorControl(void)
	{
		
		time(&begin);
		
		myRobot.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			time(&end);
			float dt = difftime(end, begin);
			time(&begin);
			
			myRobot.TankDrive(leftstick, rightstick); // drive with tank stlye uses both sticks
			
			//Control motor1 with left trigger
			if(leftstick.GetTrigger())
			{
				motor1->Set(1);
			}
			else
			{
				motor1->Set(0);
			}
			
			//Control motor2 with right trigger
			if(rightstick.GetTrigger())
			{
				motor2->Set(1);
			}
			else
			{
				motor2->Set(0);
			}
			
			//Bumper switch logic
			if(leftstick.GetRawButton(4))
			{
				 if(switch2->Get() == 0)
				 {
					 currentDir = -1;
				 }
				 
				 if(switch1->Get() == 0)
				 {
					 currentDir = 1;
				 }
				 
				 motor3->Set(currentDir);
			}
			else
			{
				currentDir = 1;
				motor3->Set(0);
			}
			
			//Solenoid in
			if(leftstick.GetRawButton(2))
			{
				s[0]->Set(true);
			}
			else
			{
				s[0]->Set(false);
			}
			
			//Solenoid out
			if(leftstick.GetRawButton(3))
			{
				s[1]->Set(true);
				
				//Start checking for a jam
				//This starts a countdown before the jam sensor is checked
				checkJam = true;
				currentJamTimer = SECONDS_JAM_SENSOR;
			}
			else
			{
				s[1]->Set(false);
			}
			
			

			//Check to see if there is a jam
			if(checkJam)
			{
				//Countdown
				currentJamTimer -= dt;
				
				//If countdown is complete
				if(currentJamTimer < 0)
				{
					//If the frisbee is still there
					if(sensor1->Get() == 1)
					{
						//Show red light
						SmartDashboard::PutBoolean("Frisbee Loader Jam", false);
					}
					else
					{
						//If frisbee is not there, it was successfully pushed, and there was no jam
						checkJam = false;
					}
				}
			}
			else
			{
				//If you aren't checking for a jam, show a green light
				SmartDashboard::PutBoolean("Frisbee Loader Jam", true);
			}
			
			//Also send raw IR sensor data to dashboard
			SmartDashboard::PutBoolean("Raw IR Sensor", (sensor1->Get() == 1));
			
		}	
		
		//Push encoder rate to dashboard
		SmartDashboard::PutNumber("Shooting Wheel Speed (ticks/sec)", encoder1->GetRate());
						
        Wait(0.005);
			
			
	}	
	
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

