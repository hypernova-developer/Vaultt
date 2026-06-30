#!/bin/bash

if [ "$EUID" -ne 0 ]; then
  echo "Please run init.sh with sudo."
  exit 1
fi

TARGET_USER=${SUDO_USER:-$(whoami)}

if [ "$TARGET_USER" = "root" ]; then
  USER_HOME="/root"
else
  USER_HOME="/home/$TARGET_USER"
fi

mkdir -p "$USER_HOME/wault"
touch "$USER_HOME/wault/passwords.txt"

chmod 000 "$USER_HOME/wault/passwords.txt"
chown root:root "$USER_HOME/wault/passwords.txt"

g++ -O3 -static -std=c++17 -DVAULTT_USER=\"$TARGET_USER\" main.cpp -o wault

if [ $? -eq 0 ]; then
  echo "--------------------------------------------------"
  echo "Compilation successful! 'wault' binary created."
  echo "Storage file initialized at: $USER_HOME/wault/passwords.txt"
  echo "Permissions restricted (chmod 000)."
  echo "Remember: You must run the 'wault' binary with sudo."
  echo "--------------------------------------------------"
else
  echo "Compilation failed."
  exit 1
fi
