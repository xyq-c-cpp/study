#!/bin/bash

rm -f /etc/systemd/system/multi-user.target.wants/server.service
rm -f /usr/lib/systemd/system/server.service

cp -f WebServer/server.service /usr/lib/systemd/system/server.service
chmod 777 /usr/lib/systemd/system/server.service

ln -sf /usr/lib/systemd/system/server.service /etc/systemd/system/multi-user.target.wants/server.service
chmod 777 /etc/systemd/system/multi-user.target.wants/server.service

rm -rf /WebServer
cp -rf WebServer /

chmod -R 777 /WebServer

systemctl start server
