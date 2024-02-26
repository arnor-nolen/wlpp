#!/bin/sh

{ 
    ./build/Debug/bin/wlpp /usr/share/wayland/wayland.xml client-header
} > ./example/wayland-client-protocol.hpp

{
    ./build/Debug/bin/wlpp /usr/share/wayland/wayland.xml private-code
} > ./example/wayland-client-protocol.cpp

./build/Debug/bin/wlpp /usr/share/wayland/wayland.xml debug

echo "Protocol files have been generated into ./example folder."

