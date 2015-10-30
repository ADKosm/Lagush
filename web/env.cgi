#!/usr/bin/python

import os

msg = ''


msg += "<font size=+1>Environment</font><br>";
for param in os.environ.keys():
  msg += "<b>%20s</b>: %s<br>" % (param, os.environ[param])

print "Content-length: " + str(len(msg))
print "Content-type: text/html\r\n";
print msg
