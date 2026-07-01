#!/bin/bash

if [ "$EUID" -eq 0 ]; then
  echo "Error: Do not run init.sh as root or with sudo."
  exit 1
fi

TARGET_USER=$(whoami)
USER_HOME="/home/$TARGET_USER"

mkdir -p "$USER_HOME/.config/vaultt"
mkdir -p "$USER_HOME/.local/bin"
touch "$USER_HOME/.config/vaultt/passwords.txt"
chmod 600 "$USER_HOME/.config/vaultt/passwords.txt"

g++ -O3 -static -std=c++17 -DVAULTT_USER=\"$TARGET_USER\" main.cpp -o vaultt

if [ $? -eq 0 ]; then
  cp vaultt "$USER_HOME/.local/bin/"
  chmod +x "$USER_HOME/.local/bin/vaultt"
  echo "--------------------------------------------------"
  echo "Compilation and installation successful!"
  echo "Binary copied to $USER_HOME/.local/bin/vaultt"
  echo "Storage file initialized at: $USER_HOME/.config/vaultt/passwords.txt"
  echo "Permissions set to 600 (Owner only)."
  echo "--------------------------------------------------"
  echo "Make sure $USER_HOME/.local/bin is in your PATH."
  echo "Now you can run Vaultt using:"
  echo "vaultt"
  echo "--------------------------------------------------"
else
  echo "Compilation failed."
  exit 1
fi

