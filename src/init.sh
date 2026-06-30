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

echo "Requesting sudo privileges to lock files and install to global PATH..."
sudo chmod 000 "$USER_HOME/vaultt/passwords.txt"
sudo chown root:root "$USER_HOME/vaultt/passwords.txt"

g++ -O3 -static -std=c++17 -DVAULTT_USER=\"$TARGET_USER\" main.cpp -o vaultt

if [ $? -eq 0 ]; then
  sudo cp vaultt /usr/local/bin/
  echo "--------------------------------------------------"
  echo "Compilation and installation successful!"
  echo "Binary copied globally to /usr/local/bin/vaultt"
  echo "Storage file initialized at: $USER_HOME/vaultt/passwords.txt"
  echo "Permissions restricted (chmod 000)."
  echo "--------------------------------------------------"
  echo "Now you can run Vaultt from anywhere using:"
  echo "sudo vaultt"
  echo "--------------------------------------------------"
else
  echo "Compilation failed."
  exit 1
fi

