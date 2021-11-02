#!/bin/bash

ulimit -c unlimited

echo "/var/coredump/core-%e-%p-%t" > /proc/sys/kernel/core_pattern

mkdir -p /var/coredump

/WebServer/server -t 2 -p 80 >/dev/null &