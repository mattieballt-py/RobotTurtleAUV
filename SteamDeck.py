# REQUIRED: pip install pygame

"""
Steam Deck Python script to run to send input signals to Arduino
"""

import pygame
import socket
import time

# --- Network config ---
ARDUINO_IP = "192.168.1.50"
ARDUINO_PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# --- Controller setup ---
pygame.init()
pygame.joystick.init()

joystick = pygame.joystick.Joystick(0)
joystick.init()

def clamp(val, lo, hi):
    return max(lo, min(hi, val))

while True:
    pygame.event.pump()

    # Example mappings (adjust to taste)
    motor_button = joystick.get_button(0)   # A button
    left_axis = joystick.get_axis(1)         # Left stick Y
    right_axis = joystick.get_axis(4)        # Right stick Y

    # Convert axis (-1..1) to servo angle (0..180)
    left_angle = int(clamp((left_axis + 1) * 90, 0, 180))
    right_angle = int(clamp((right_axis + 1) * 90, 0, 180))

    motor_on = 1 if motor_button else 0

    packet = f"M:{motor_on},L:{left_angle},R:{right_angle}"
    sock.sendto(packet.encode(), (ARDUINO_IP, ARDUINO_PORT))

    time.sleep(0.02)  # 50 Hz update rate
