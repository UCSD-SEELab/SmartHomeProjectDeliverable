import numpy as np
import wiringpi as wp
from datetime import datetime
import time
from picamera import PiCamera
import imageio

class pir:
    def __init__(self, pin_pir):
        self.pin_pir = pin_pir
        wp.wiringPiSetupGpio() # use GPIO pin numbering
        for pir in self.pin_pir:
            wp.pinMode(pir, 0) # input

    def read_pir(self):
        values = []
        for pir in self.pin_pir:
            values.append(wp.digitalRead(pir))
        return values

class cam:
    def __init__(self):
        self.cam = PiCamera()
        self.cam.framerate = 24
        self.cam.rotation = 180
        self.cam.resolution = (1640, 922)
        time.sleep(0.1)

    def record_video(self,secs):
        timestamp = str(datetime.now())
        self.cam.start_recording(timestamp+'.h264', format='h264', quality=23)
        self.cam.annotate_text = timestamp
        time.sleep(secs)
        self.cam.stop_recording()

    def take_picture(self,name):
        self.cam.capture(name)

    def make_gif(self, filenames, save_name):
        images = []
        for filename in filenames:
            images.append(imageio.imread(filename))
        imageio.mimsave(save_name,images)


if __name__ == "__main__":
    pir = pir([23,24,25,27,22])
    cam = cam()

    while True:
        if not np.all(pir.read_pir() == [0, 0, 0, 0, 0]):
            timestamp = str(datetime.now())
            cam.take_picture("/picture/"+timestamp+".png")
            print pir.read_pir()
            time.sleep(0.3)
