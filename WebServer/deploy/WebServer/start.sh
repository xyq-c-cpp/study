#!/bin/bash

if [ ! $# -eq 1 ]; then
  echo "args: [start | stop | restart]"
  exit
fi

function stopProc(){
  pid=$(ps -ef|grep /WebServer/server|grep -v grep|awk '{print $2}')
  if [ ! ${pid} == "" ]; then
    kill -9 ${pid}
  fi
}

function startProc(){
  ulimit -c unlimited
    
  echo "/var/coredump/core-%e-%p-%t" > /proc/sys/kernel/core_pattern

  mkdir -p /var/coredump

  /WebServer/server -t 2 -p 80 >/dev/null &
}

case $1 in
  "stop")
    stopProc
  ;;
    
  "start")
    startProc
  ;;
  
  "restart")
    stopProc
    startProc
  ;;
  
  "*")
    echo "invalid args"
    echo "usage: [start | stop | restart]"
  ;;
esac

