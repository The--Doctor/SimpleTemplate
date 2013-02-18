#define GAMEPAD_A 2
#define GAMEPAD_B 3
#define GAMEPAD_X 1
#define GAMEPAD_Y 4
	
#define GAMEPAD_BACK 9
#define GAMEPAD_START 10

#define GAMEPAD_LEFT_CLICK 11
#define GAMEPAD_RIGHT_CLICK 12
	
#define GAMEPAD_L_TRIGGER 7
#define GAMEPAD_R_TRIGGER 8
	
#define GAMEPAD_L_BUMPER 5
#define GAMEPAD_R_BUMPER 6

#define GAMEPAD_L_STICK_X 1
#define GAMEPAD_L_STICK_Y 2
#define GAMEPAD_R_STICK_X 3
#define GAMEPAD_R_STICK_Y 4

	
	bool Solenoid1In()
	{
		return ( leftstick.GetRawButton(2) || gamepad.GetRawButton( GAMEPAD_A ) ); //Button A
	}
	
	bool Solenoid1Out()
	{
		return (leftstick.GetRawButton(3) || gamepad.GetRawButton( GAMEPAD_B ) ); //Button B
	}
	
	bool Solenoid2In()
	{
		return (rightstick.GetRawButton(2) || gamepad.GetRawButton( GAMEPAD_X ) ); //Button X
	}
	
	bool Solenoid2Out()
	{
		return (rightstick.GetRawButton(3) || gamepad.GetRawButton( GAMEPAD_Y ) ); //Button Y
	}
	
	bool ShooterControl()
	{
		return (leftstick.GetTrigger() || rightstick.GetTrigger() || gamepad.GetRawButton( GAMEPAD_L_TRIGGER ) || gamepad.GetRawButton( GAMEPAD_R_TRIGGER ) );
		//Either trigger on the joysticks or gamepad will activate the shooter
	}
	
	bool ArmLeft()
	{
		return (leftstick.GetRawButton(4) || gamepad.GetRawButton( GAMEPAD_L_BUMPER ) );
	}
	
	bool ArmRight()
	{
		return (rightstick.GetRawButton(4) || gamepad.GetRawButton( GAMEPAD_R_BUMPER) );
	}
	
	bool SwitchSpeed()
	{
		return (leftstick.GetRawButton(5) || gamepad.GetRawButton( GAMEPAD_BACK ));
	}
	/*s
	
	float LeftDriveY(bool gamepad)
	{
		return leftstick.GetY() + gamepad.GetRawAxis(GAMEPAD_L_STICK_Y);
	}
	float RightDriveY(bool gamepad)
	{
		return rightstick.GetY() + gamepad.GetRawAxis(GAMEPAD_R_STICK_Y);
	}
	
	float LeftDriveX(bool gamepad)
	{
		return leftstick.GetX() + gamepad.GetRawAxis(GAMEPAD_L_STICK_X);
	}
	float RightDriveX(bool gamepad)
	{
		return rightstick.GetX() + gamepad.GetRawAxis(GAMEPAD_R_STICK_X);
	}
	*/
