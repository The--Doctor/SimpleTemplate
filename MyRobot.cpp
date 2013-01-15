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
	Joystick rightstick; // Right joystick
	Joystick leftstick; //Left Stick
	Jaguar *motor1;
	Jaguar *motor2;
	
	Jaguar *motor3;
	
	DigitalInput *switch1;
	
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
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(-0.5, 0.0); 	// drive forwards half speed
		Wait(2.0); 				//    for 2 seconds
		myRobot.Drive(0.0, 0.0); 	// stop robot
	}

	/**
	 * Runs the motors with arcade steering. 
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
			};
			
			if(rightstick.GetTrigger())
			{
				motor2->Set(1);
			}
			else
			{
				motor2->Set(0);
			}
			
			if(switch1->Get() == 1)
			{
				motor3->Set(1);
			}
			else
			{
				motor3->Set(0);
			}
						
         Wait(0.005);
			
			
		  
		}	
	}
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
};

START_ROBOT_CLASS(RobotDemo);

