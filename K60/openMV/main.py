# main.py -- put your code here!
import pyb, time,sensor,image
from pyb import UART

green_led = pyb.LED(2)

green_led .off()                          # 关闭所有LED
color  = (   0,   45,  -20,   40,   -4 0,  0)
uart = UART(3, 115200)
uart.init(115200, bits=8, parity=None, stop=1)
sensor.reset()                          # 初始化摄像头
sensor.set_pixformat(sensor.RGB565)     # use RGB565.
sensor.set_framesize(sensor.QQVGA)      # use QQVGA for speed.
sensor.set_windowing((80, 80))
sensor.skip_frames(10)                  # 跳过10帧
sensor.set_auto_gain(False)             # must be turned off for color tracking
sensor.set_auto_whitebal(False)         # 关闭白平衡。白平衡是默认开启的，在颜色识别中，需要关闭白平衡。
clock = time.clock()                    # 追踪 FPS.

while(True):
    clock.tick() # Track elapsed milliseconds between snapshots().
    img = sensor.snapshot() # Take a picture and return the image.

    blobs = img.find_blobs([color])
    #find_blobs(thresholds, invert=False, roi=Auto),thresholds为颜色阈值，
    #是一个元组，需要用括号［ ］括起来。invert=1,反转颜色阈值，invert=False默认
    #不反转。roi设置颜色识别的视野区域，roi是一个元组， roi = (x, y, w, h)，代表
    #从左上顶点(x,y)开始的宽为w高为h的矩形区域，roi不设置的话默认为整个图像视野。
    #这个函数返回一个列表，[0]代表识别到的目标颜色区域左上顶点的x坐标，［1］代表
    #左上顶点y坐标，［2］代表目标区域的宽，［3］代表目标区域的高，［4］代表目标
    #区域像素点的个数，［5］代表目标区域的中心点x坐标，［6］代表目标区域中心点y坐标，
    #［7］代表目标颜色区域的旋转角度（是弧度值，浮点型，列表其他元素是整型），
    #［8］代表与此目标区域交叉的目标个数，［9］代表颜色的编号（它可以用来分辨这个
    #区域是用哪个颜色阈值threshold识别出来的）。
    if blobs:
    #如果找到了目标颜色
        for b in blobs:
        #迭代找到的目标颜色区域
            # Draw a rect around the blob.
            img.draw_rectangle(b[0:4]) # rect
            #用矩形标记出目标颜色区域
            img.draw_cross(b[5], b[6]) # cx, cy
            #在目标颜色区域的中心画十字形标记
            uart.writechar(0xFF)
            uart.writechar(0xEE)
            uart.writechar(b[5])
            uart.writechar(b[6])
            green_led.toggle()
    print(clock.fps())
           # uart.write(b[5], b[6])
            #输出目标物体中心坐标

  # print(clock.fps()) # Note: Your OpenMV Cam runs about half as fast while
 

    # connected to your computer. The FPS should increase once disconnected.