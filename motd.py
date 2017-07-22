#!/usr/bin/env python3

from socket import *
import sys

sock = socket(AF_INET, SOCK_STREAM)
try:
  sock.connect(("ggolish.zapto.org", 2402))
except:
  sys.exit(0)

print(sock.recv(4096).decode())

