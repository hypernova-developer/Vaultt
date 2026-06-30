#!/bin/bash

if [ "$EUID" -eq 0 ]; then
  echo "Error: Do not run init.sh as root or with sudo."
  echo "Run it as a regular user so it can correctly detect your home directory."
  exit 1
fi

TARGET_USER=$(whoami)
USER_HOME="/home/$TARGET_USER"

mkdir -p "$USER_HOME/vaultt"
touch "$USER_HOME/vaultt/passwords.txt"

echo "Requesting sudo privileges to lock the storage file permissions..."
sudo chmod 000 "$USER_HOME/vaultt/passwords.txt"
sudo chown root:root "$USER_HOME/vaultt/passwords.txt"

g++ -O3 -static -std=c++17 -DVAULTT_USER=\"$TARGET_USER\" main.cpp -o vaultt

if [ $? -eq 0 ]; then
  echo "--------------------------------------------------"
  echo "Compilation successful! 'vaultt' binary created."
  echo "Storage file initialized at: $USER_HOME/vaultt/passwords.txt"
  echo "Permissions restricted (chmod 000)."
  echo "Remember: You must run the 'vaultt' binary with sudo."
  echo "--------------------------------------------------"
else
  echo "Compilation failed."
  exit 1
fi

