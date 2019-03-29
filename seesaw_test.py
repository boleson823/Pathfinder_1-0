import Adafruit_Seesaw
import time

MOTORA1_PIN = 10
MOTORA2_PIN = 9
MOTORB1_PIN = 11
MOTORB2_PIN = 14
MOTORA_PWM = 5
MOTORB_PWM = 6

ss = Adafruit_Seesaw.Seesaw()

ss.pin_mode(MOTORA1_PIN, ss.OUTPUT)
ss.pin_mode(MOTORA2_PIN, ss.OUTPUT)
ss.pin_mode(MOTORB1_PIN, ss.OUTPUT)
ss.pin_mode(MOTORB2_PIN, ss.OUTPUT)

#ss.pin_mode(MOTORA_PWM, ss.OUTPUT)
#ss.pin_mode(MOTORB_PWM, ss.OUTPUT)

ss.digital_write(MOTORA1_PIN, False)
ss.digital_write(MOTORA2_PIN, True)
ss.digital_write(MOTORB1_PIN, False)
ss.digital_write(MOTORB2_PIN, True)

j = 0

while j < 5:
	ss.analog_write(MOTORA_PWM, 0)
	time.sleep(0.1)
	ss.analog_write(MOTORB_PWM, 100)
	time.sleep(1)
	ss.analog_write(MOTORA_PWM, 100)
	time.sleep(0.1)
	ss.analog_write(MOTORB_PWM, 0)
	time.sleep(1)
	j += 1

ss.analog_write(MOTORA_PWM, 0)
