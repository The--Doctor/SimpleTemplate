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
	Victor *motor1;
	Victor *motor2;	
	Victor *motor3;
	DigitalInput *switch1;
	DigitalInput *switch2;
	DigitalInput *sensor1;
	Encoder *encoder1;
	Encoder *encoder2;
	Encoder *encoder3;
	Compressor *c;
	
	
#define NUM_S 2
	Solenoid *s[NUM_S];
	
	float currentDir;
	
#define SECONDS_JAM_SENSOR 3
	
#define CPR 360
	
#define NUM_SPEEDS 3	
	int speedIndex;
#define USING_ENCODER

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
		motor1 = new Victor(3); //PWM Channel 3
		motor2 = new Victor(4); //PWM Channel 4
		motor3 = new Victor(5); //PWM Channel 5
		
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
		
		//Start at speed index 0
		speedIndex = 0;
		
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		myRobot.SetSafetyEnabled(false);
		myRobot.Drive(-0.5, 0.0); 	// drive forwards half speed
		Wait(5.000);		
			
		
		myRobot.Drive(0.0, 0.0); 	// stop robot
		
		
	}

	/**
	 * Runs the motors with Tank steering. 
	 */
	
	
	void OperatorControl(void)
	{
		float SPEEDS[NUM_SPEEDS] = {0.0f, 0.5f, 1.0f};
		
		bool leftbutton5 = false;
		bool rightbutton5 = false;
		
		float currentShooterSpeed = 0.0f;
		
		time(&begin);
		
		myRobot.SetSafetyEnabled(true);
		while (IsOperatorControl())
		{
			time(&end);
			float dt = difftime(end, begin);
			time(&begin);
			
			//Switching speed indexes
			//This fancy stuff is to make sure that the speed only switches when the button is first pressed
			if(leftbutton5 == false && leftstick.GetRawButton(5) == true)
			{
				++speedIndex;
				
				if(speedIndex >= NUM_SPEEDS)
				{
					speedIndex = 0;
				}
			}
			leftbutton5 = leftstick.GetRawButton(5);
			
			//Print information about joystick axes
			SmartDashboard::PutNumber("Left Stick:", leftstick.GetY());
			SmartDashboard::PutNumber("Right Stick:", rightstick.GetY());
			
			myRobot.TankDrive(leftstick, rightstick); // drive with tank stlye uses both sticks
			
			//When both trigger ar pressed motor1 does not move
			if(leftstick.GetTrigger() && rightstick.GetTrigger())
			{
				motor1->Set(0);
			}
			else
			{
			
				//Control motor1 with left trigger
				if(leftstick.GetTrigger())
				{
					motor1->Set(1);
				}
				else
				{
					//invert motor1 with right trigger
					if(rightstick.GetTrigger())
					{
						motor1->Set(-1);
					}
					else
					{
						motor1->Set(0);
					}
				}
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
			
			//Push encoder 1 rate to dashboard
			SmartDashboard::PutNumber("Encoder 1 (revs/sec):", (encoder1->GetRate())/CPR  );
			
			//Push encoder 2 rate to dashboard
			SmartDashboard::PutNumber("Encoder 2 (revs/sec):", (encoder2->GetRate())/CPR  );
			
			//Push encoder 3 rate to dashboard
			SmartDashboard::PutNumber("Encoder 3 (revs/sec):", (encoder3->GetRate())/CPR  );
			
			
			#ifdef USING_ENCODER
				//Push expected shooter speed
				SmartDashboard::PutNumber("Expected Shooter Speed (revs/sec):", (SPEEDS[speedIndex])/CPR  );
				
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
			#else
				//Push expected shooter speed
				SmartDashboard::PutNumber("Expected Shooter Speed (Joystick Axis):", SPEEDS[speedIndex]);
				if( rightstick.GetRawButton(5) )
				{
					motor2->Set(SPEEDS[speedIndex]);
				}
				else
				{
					motor2->Set(0);
				}
			#endif
							
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

