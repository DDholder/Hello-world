import sensor, image, time,pyb
from pyb import UART
import json
green_threshold   = (   7,   120)
uart = UART(3,115200,timeout_char = 1000000)
ROI=(51,18,196,196)
sensor.reset()
sensor.set_pixformat(sensor.GRAYSCALE)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing(ROI)
sensor.skip_frames(10)
sensor.set_auto_gain(True)
sensor.set_auto_whitebal(False)
clock = time.clock()
led = pyb.LED(3)
while(True):
	clock.tick()
	img = sensor.snapshot()
	blobs = img.find_blobs([green_threshold],x_stride=2,y_stride=2,area_threshold=6)
	led.on()
	if blobs:
		for b in blobs:
			img.draw_cross(b[5],b[6])
			print(b[5],b[6])
                        uart.writechar(0xFF)
                        uart.writechar(0xEE)
                        uart.writechar(b[5])
                        uart.writechar(b[6])	
	print(clock.fps())