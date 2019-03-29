import Adafruit_Seesaw
import time, os, sys, termios, tty

def getch():
	fd = sys.stdin.fileno()
	old_settings = termios.tcgetattr(fd)
	try:
		tty.setraw(sys.stdin.fileno())
		ch = sys.stdin.read(1)
	finally:
		termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)

	return ch

def robot_move():
	mASpd = 0
	mBSpd = 0

	while True:
		keyPres = getch()
		print keyPres
		if keyPres == "w":	#up key
			mASpd += 10
			mBSpd += 10
			if mASpd > 255:
				mASpd = 255
			if mBSpd > 255:
				mBSpd = 255
		elif keyPres == "s":	#down key
			mASpd -= 10
			mBSpd -= 10
			if mASpd < 0:
				mASpd = 0
			if mBSpd < 0:
				mBSpd = 0
		elif keyPres == "d":	#right key
			mASpd -= 10
			mBSpd += 10
			if mBSpd >255:
				mBSpd = 255
			if mASpd < 0:
				mASpd = 0
		elif keyPres == "a":	#left key
			mASpd += 10
			mBSpd -= 10
			if mBSpd < 0:
				mBSpd = 0
			if mASpd > 255:
				mASpd = 255
		elif keyPres == "b":
			mBSpd = 0
			mASpd = 0
		elif keyPres == "q":
			break
		else:
			print("No Control Key Detected")
		ss.analog_write(MOTORA_PWM, mASpd)
		time.sleep(0.05)
		ss.analog_write(MOTORB_PWM, mBSpd)


ss = Adafruit_Seesaw.Seesaw()

MOTORA_PIN = [10, 9]
MOTORB_PIN = [11, 14]
MOTORA_PWM = 5
MOTORB_PWM = 6

if __name__ == '__main__':

	for i in MOTORA_PIN:
		ss.pin_mode(i, ss.OUTPUT)

	for i in MOTORB_PIN:
		ss.pin_mode(i, ss.OUTPUT)

	ss.digital_write(MOTORA_PIN[0], False)
	ss.digital_write(MOTORA_PIN[1], True)
	ss.digital_write(MOTORB_PIN[0], False)
	ss.digital_write(MOTORB_PIN[1], True)

	robot_move()


