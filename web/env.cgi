#!/usr/bin/python

import os

msg = ''

msg += "<font size=+1>Environment</font><br>";
for param in os.environ.keys():
  msg += "<b>%20s</b>: %s<br>" % (param, os.environ[param])

msg += """
<form action="/cgi-bin/hello_get.py" method="post">
First Name: <input type="text" name="first_name"><br />
Last Name: <input type="text" name="last_name" />

<input type="submit" value="Submit" />
</form>
"""

print "Content-length: " + str(len(msg))
print "Content-type: text/html\r\n";
print msg
