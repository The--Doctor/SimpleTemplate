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
	Compressor *c;
	
	
#define NUM_S 2
	Solenoid *s[NUM_S];
	
	float currentDir;

	
public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		leftstick(1), // These must be declared above
		rightstick(2) //tank drive
		
	{
		myRobot.SetExpiration(0.1);
		motor1 = new Jaguar(3);
		motor2 = new Jaguar(4);
		motor3 = new Jaguar(5);
		switch1 = new DigitalInput(1);
		switch2 = new DigitalInput(2);
		c = new Compressor(3, 1);
		c->Start();
		
		for(int i = 0; i < NUM_S; ++i)
		{
			s[i] = new Solenoid(i + 1);
			s[i]->Set(false);
		}
		
		currentDir = 1;
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
		
		
		
		myRobot.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			
			myRobot.TankDrive(leftstick, rightstick); // drive with tank stlye uses both sticks
			if(leftstick.GetTrigger())
			{
				motor1->Set(1);
			}
			else
			{
				motor1->Set(0);
			}
			
			if(rightstick.GetTrigger())
			{
				motor2->Set(1);
			}
			else
			{
				motor2->Set(0);
			}
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
			}
			else
			{
				s[1]->Set(false);
			}
		
		}	
						
         Wait(0.005);
			
			
	}	
	
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

