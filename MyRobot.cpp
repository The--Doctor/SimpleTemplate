#include "WPILib.h"
#include "Vision/AxisCamera.h"
#include "nivision.h"
#include "math.h"

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
	
	Joystick gamepad;
	
	Jaguar *motor1;
	Jaguar *motor2;	
	Jaguar *motor3;
	Jaguar *motor4;
	DigitalInput *switch1;
	DigitalInput *switch2;
	DigitalInput *sensor1;
	Encoder *encoder1;
	Encoder *encoder2;
	Encoder *encoder3;
	Compressor *c;
	Relay *camlight;
	
	
#define NUM_S 4
	Solenoid *s[NUM_S];
	
	float currentDir;
	
#define SECONDS_JAM_SENSOR 3
	
#define CPR 360
	
#define NUM_SPEEDS 3	
	int speedIndex;
	
#define USING_ENCODER false

#define CLIMB_SPEED 0.3
	
	float currentJamTimer;
	bool checkJam;
	
	time_t begin, end;

public:
	RobotDemo(void):
		myRobot(1, 2),	// these must be initialized in the same order
		leftstick(1), // These must be declared above
		rightstick(2), //tank drive
		gamepad(3)
		
	{
		myRobot.SetExpiration(0.1);
		
		//Register 3 extra motors (in addition to the two drive motors)
		motor1 = new Jaguar(3); //PWM Channel 3
		motor2 = new Jaguar(4); //PWM Channel 4
		motor3 = new Jaguar(5); //PWM Channel 5
		motor4 = new Jaguar(6); //PWM Channel 6
		
		//Two Bumber switches
		switch1 = new DigitalInput(1); //Digital input port 1
		switch2 = new DigitalInput(2); //Digital input port 2
		
		//IR Sensor, Digital input port 4
		sensor1 = new DigitalInput(4);
		
		//Encoder 1
		encoder1 = new Encoder(5, 6, true); //A channel - Digital Input 5, B Channel - Digital Input 6
		encoder1->Start(); //Start counting
		encoder2 = new Encoder(7, 8, true); //A channel - Digital Input 7, B Channel - Digital Input 8
		encoder2->Start(); // Start counting
		encoder3 = new Encoder(9, 10, true); //A channel - Digital Input 9, B Channel - Digital Input 10
		encoder3->Start(); // Start counting
		
	
		
		//Compressor
		c = new Compressor(3, 1); //Pressure switch connected to Digital Input 3
		//Spike relay conencted to relay port 1
		c->Start();
		
		//Spike Relay
		camlight = new Relay(11);
		camlight ->Set(Relay::kOn);
		
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
		
		//Start at speed index 0
		speedIndex = 0;

		
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		//Initial speeds for left and right motors
		double left = 0.5;
		double right = 0.5;
		double timer = 0.0;
		//Whether or not to adjust right wheel
		bool adjustRightWheel = true;
		
		myRobot.SetSafetyEnabled(false);
		while (IsAutonomous() && IsEnabled())
		{
			//Send command to motors
			myRobot.Drive(left, right);
			if(timer < 2)
			{
				//Get left and right motor speeds from encoder1 and encoder2 respectively
				double leftRate = encoder1->GetRate();
				double leftdistance = encoder1->GetDistance();
				double rightRate = encoder2->GetRate();
				double rightdistance = encoder2->GetDistance();
				
						
				//Push encoder 1 rate/distance to dashboard
				SmartDashboard::PutNumber("Encoder 1",  leftRate);
				SmartDashboard::PutNumber("Encoder 1", leftdistance );
				
				//Push encoder 2 rate/distance to dashboard
				SmartDashboard::PutNumber("Encoder 2", rightRate );
				SmartDashboard::PutNumber("Encoder 2", rightdistance );
				
				//Increment to adjust by every cycle
				double increment = 0.01;
				
				//The acceptable error, within which no more adjustment is required
				double acceptableError = 0;
				
				//If you want to adjust the right wheel speed
				if(adjustRightWheel)
				{
					//If the right wheel's speed is greater than the left wheel, plus some error margin
					if(rightRate > leftRate + acceptableError)
					{
						//Add a small amount to the right wheel's speed
						right -= increment;
					}
					
					//If the right wheel's speed is less than the left wheel, minus some error margin
					if(rightRate < leftRate - acceptableError)
					{
						//Subtract a small amount from the right wheel's speed
						right += increment;
					}
				}
			//Wait some time before recalculating
			Wait(0.05);
		
			}
			else
			{
				myRobot.Drive(0.0, 0.0);
			}
			
			timer+=0.05;
		}
			
		
		
		myRobot.Drive(0.0, 0.0);
		
	}

	/**
	 * Runs the motors with Tank steering. 
	 */
	
#include "input.h"
	
	void OperatorControl(void)
	{
		//Start out in arcade drive mode
		SmartDashboard::PutBoolean("TankDrive", false);
		
		
		float SPEEDS[NUM_SPEEDS] = {0.2f, 0.5f, 1.0f};
		
		bool leftbutton5 = false;
		bool rightbutton5 = false;
		
		bool l4 = false;
		bool r4 = false;
		
		float currentShooterSpeed = 0.0f;
		
		time(&begin);
		
		myRobot.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			
			//Measure elapsed time
			time(&end);
			float dt = difftime(end, begin);
			time(&begin);

			//Switching speed indexes
			//This fancy stuff is to make sure that the speed only switches when the button is first pressed
			if(leftbutton5 == false && SwitchSpeed() == true)
			{
				++speedIndex;
				
				if(speedIndex >= NUM_SPEEDS)
				{
					speedIndex = 0;
				}
			}
			leftbutton5 = SwitchSpeed();
			
			//Push expected shooter speed
			//SmartDashboard::PutNumber("Expected Shooter Speed", SPEEDS[speedIndex] - gamepad.GetRawAxis(GAMEPAD_R_STICK_Y));
			
			SmartDashboard::PutNumber("Expected Shooter Speed", (-1.0*rightstick.GetThrottle() + 1 ) * 0.5f);
			
			//Print information about joystick axes
			SmartDashboard::PutNumber("Left Stick:", leftstick.GetY()*50+50);
			SmartDashboard::PutNumber("Right Stick:", rightstick.GetY()*50+50);
			
			
			if(SmartDashboard::GetBoolean("TankDrive"))
			{
				myRobot.TankDrive( leftstick, rightstick ); // drive with tank stlye uses both sticks
			}
			else
			{
				myRobot.ArcadeDrive( leftstick.GetY() , leftstick.GetX() );
			}
			
			//When both triggers are pressed motor1 does not move
			if(ArmLeft() && ArmRight())
			{
				motor1->Set(0);
			}
			else
			{
				if(ArmLeft() || ArmRight())
				{
					motor1->Set(currentDir * CLIMB_SPEED);
				}
				else
				{
					motor1->Set(0);
				}
			
				//When left trigger is first pressed
				if(ArmLeft() && l4 == false)
				{
					currentDir = -1;
					l4 = true;
				}
				
				//When right trigger is first pressed
				if(ArmRight() && r4 == false)
				{
					currentDir = 1;
					r4 = true;
				}
				
				if(switch1->Get() == 0)
				{
					currentDir = -1;
				}
				if(switch2->Get() == 0)
				{
					currentDir = 1;
				}
			}
			l4 = ArmLeft();
			r4 = ArmRight();
			
			SmartDashboard::PutNumber("Expected Arm Direction:", currentDir  );
			
			//Bumper switch 1 dashboard
			if(switch1->Get() == 0)
			{
				SmartDashboard::PutBoolean("Bumper Switch 1 Raw:", true  );
			}
			else
			{
				SmartDashboard::PutBoolean("Bumper Switch 1 Raw:", false  );
			}
			
			//Bumper switch 2 dashboard
			if(switch2->Get() == 0)
			{
				SmartDashboard::PutBoolean("Bumper Switch 2 Raw:", true  );
			}
			else
			{
				SmartDashboard::PutBoolean("Bumper Switch 2 Raw:", false  );
			}
			
			//Solenoid in
			if( Solenoid1In() )
			{
				s[0]->Set(true);
			}
			else
			{
				s[0]->Set(false);
			}
			
			//Solenoid out
			if( Solenoid1Out() )
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
			
			//Solenoid 2 in
			if( Solenoid2In() )
			{
				s[2]->Set(true);
			}
			else
			{
				s[2]->Set(false);
			}
			//Solenoid 2 out
			if( Solenoid2Out() )
			{
				s[3]->Set(true);
			}
			else
			{
				s[3]->Set(false);
			}
			
			SmartDashboard::PutNumber("Jam Timer", currentJamTimer);

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
			if(sensor1->Get() == 1)
			{
				SmartDashboard::PutBoolean("Raw IR Sensor", true);
			}
			else
			{
				SmartDashboard::PutBoolean("Raw IR Sensor", false);
			}
			
			//SmartDashboard::PutNumber("Left Throttle", leftstick.GetThrottle());
			
			if(USING_ENCODER)
			{
				//Push expected shooter speed
				SmartDashboard::PutNumber("Expected Shooter Speed", SPEEDS[speedIndex]  );
				
				if(rightbutton5 == false && rightstick.GetRawButton(5) == true)
				{
					//When button 5 on the right stick is first pressed, set the shooterspeed to 0.5,
					//On a scale of -1.0 to 1.0
					currentShooterSpeed = 0.5f;
				}
				rightbutton5 = rightstick.GetRawButton(5);
				
				if(rightstick.GetRawButton(5))
				{
					//Assume that encoder 3 is the encoder attached to the shooter
					
					//May need to have a buffer period before starting to adjust speed?
					
					//If the encoder's rate is less than the desired rev/sec, increase currentShooterSpeed
					if((encoder3->GetRate())/CPR < SPEEDS[speedIndex])
					{
						currentShooterSpeed += 0.01f;
					}
					//If the encoder's rate is greated than the desired rev/sec, decrease currentShooterSpeed
					if((encoder3->GetRate())/CPR > SPEEDS[speedIndex])
					{
						currentShooterSpeed -= 0.01f;
					}
					
					//Send that value to the motor
					motor2->Set(currentShooterSpeed);
				}
			}
			else
			{
				
				
				if( ShooterControl() )
				{
					motor2->Set( (-1.0*rightstick.GetThrottle() + 1 ) * 0.5f );
				}
				else
				{
					motor2->Set(0);
				}
			}
							
			Wait(0.005);
		}	
			
	}	
	
	
	/**
	 * Runs during test mode
	 */
	void Test() 
	{
		
	}
  };

START_ROBOT_CLASS(RobotDemo);

//EOF
