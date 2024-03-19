# -*- coding: utf-8 -*-
import tkinter as tk
import serial
import serial.tools.list_ports
import pygame
import random
import sys
import re
import os

class MusicalAgent:
	def __init__(self, master):
		self.master = master
		self.master.title("Tone Generator Agent")
		
		# Serial setup
		self.ser = None
		self.baud_var = tk.StringVar()
		self.com_port_var = tk.StringVar()
		
		# Pygame setup
		pygame.mixer.init()
		self.channels = [pygame.mixer.Channel(i) for i in range(8)]  # Initialize 8 channels
		
		# GUI setup
		self.setup_gui()
		
		# Open serial connection
		self.open_serial()

	def setup_gui(self):
		# Tones to include
		tones = ["C3", "D3", "E3", "F3", "G3", "A4", "B4", "C4"]
		
		# Create buttons for playing tones
		self.tone_buttons = []
		for i, tone in enumerate(tones):
			tone_button = tk.Button(self.master, text=f"Tone {tone}", command=lambda t=tone: self.play_tone(t))
			tone_button.grid(row=0, column=i)
			self.tone_buttons.append(tone_button)
   
		stop_button = tk.Button(self.master, text="Stop Tone", command=self.stop_tone)
		stop_button.grid(row=4, column=0, columnspan=len(tones))
		
		# Baudrate selection
		baud_label = tk.Label(self.master, text="Baudrate:")
		baud_label.grid(row=1, column=0)
		baud_entry = tk.Entry(self.master, textvariable=self.baud_var)
		baud_entry.grid(row=1, column=1)
		self.baud_var.set("115200")  # Default baudrate
		
		# COM port selection
		com_port_label = tk.Label(self.master, text="COM Port:")
		com_port_label.grid(row=2, column=0)
		com_port_entry = tk.Entry(self.master, textvariable=self.com_port_var)
		com_port_entry.grid(row=2, column=1)
		self.com_port_var.set("COM6")  # Default COM port
		
		# Open serial button
		open_button = tk.Button(self.master, text="Open Serial", command=self.open_serial)
		open_button.grid(row=1, column=2, rowspan=2)
		
		# Message display
		self.message_display = tk.Text(self.master, width=50, height=10)
		self.message_display.grid(row=3, columnspan=10)
		
	def play_tone(self, tone):
		# Play the tone from the corresponding .mp3 file
		file_path = os.path.join("Tones", f"{tone.lower()}.mp3")
		if os.path.exists(file_path):
			for channel in self.channels:
				if not channel.get_busy():  # Check if channel is not playing any sound
					sound = pygame.mixer.Sound(file_path)
					channel.play(sound)
					break  # Exit loop after playing the sound on the first available channel
		else:
			file_path = os.path.join("Tones", f"{tone.lower()}.wav")
			if os.path.exists(file_path):
				for channel in self.channels:
					if not channel.get_busy():  # Check if channel is not playing any sound
						sound = pygame.mixer.Sound(file_path)
						channel.play(sound)
						break  # Exit loop after playing the sound on the first available channel
			else:
				self.display_message(f"File {tone}.mp3 not found.")
   
	def stop_tone(self):
		# Stop currently playing tone on all channels
		for channel in self.channels:
			channel.stop()

	def open_serial(self):
		# Open serial connection
		baudrate = int(self.baud_var.get())
		com_port = self.com_port_var.get()
		
		try:
			self.ser = serial.Serial(port=com_port, baudrate=baudrate)
			self.display_message(f"Serial connection opened: {com_port}, Baudrate: {baudrate}")
		except serial.SerialException as e:
			self.display_message(f"Failed to open serial connection: {e}")

	def display_message(self, message):
		# Display message in the message display
		self.message_display.insert(tk.END, message + "\n")
		self.message_display.see(tk.END)

	def read_serial(self):
		# Read from serial port
		if self.ser and self.ser.is_open:
			try:
				data = b""
				while self.ser.in_waiting > 0:  # Read until there is data available
					data += self.ser.read()
				if data:
					messages = data.decode("utf-8").strip().split("\n")  # Split data into separate messages
					for message in messages:
						if message == "Tacet":
							self.stop_tone()  # Stop tone if "Tacet" is received
							self.display_message("Tone stopped.")
						elif message.endswith('.mp3') or message.endswith('.wav'):
							tone = message[:-4]  # Remove '.mp3' extension
							self.display_message(f"Received: {message}")
							self.play_tone(tone)  # Play tone based on received message
						else:
							self.display_message(f">> {message}")
			except Exception as e:
				self.display_message(f"Error reading from serial: {e}")

	def refresh(self):
		# Continuously read from serial port
		self.read_serial()
		self.master.after(100, self.refresh)

if __name__ == "__main__":
	root = tk.Tk()
	musical_agent = MusicalAgent(root)
	root.after(100, musical_agent.refresh)  # Start refreshing
	root.mainloop()
