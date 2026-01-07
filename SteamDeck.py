# REQUIRED: pip install pygame

"""
Steam Deck Python script to run to send input signals to Arduino
"""
import pygame
import socket
import time

ARDUINO_IP = "192.168.1.50"
PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

pygame.init()
pygame.joystick.init()
js = pygame.joystick.Joystick(0)
js.init()

def map_axis(val, out_min, out_max):
    return int((val + 1) / 2 * (out_max - out_min) + out_min)

while True:
    pygame.event.pump()

    # Right trigger → forward speed
    trigger = js.get_axis(5)       # 0..1 (Steam Deck)
    speed = int(trigger * 255)

    # Left stick X → yaw (front + rear flippers)
    yaw = js.get_axis(0)
    flipper_angle = map_axis(yaw, 60, 120) if abs(yaw) > 0.1 else 90

    # Right stick X → neck yaw
    neck_axis = js.get_axis(2)
    neck_angle = map_axis(neck_axis, 60, 120) if abs(neck_axis) > 0.1 else 90

    packet = f"SPD:{speed},FL:{flipper_angle},FR:{flipper_angle},NECK:{neck_angle}"
    sock.sendto(packet.encode(), (ARDUINO_IP, PORT))

    time.sleep(0.02)
