#!/usr/bin/python

import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
first_name = form.getvalue('first_name')
last_name  = form.getvalue('last_name')

msg = ""

msg+= "<html>"
msg+= "<head>"
msg+= "<title>Hello - First CGI Program</title>"
msg+= "</head>"
msg+= "<body>"
msg+= "<h2 align = 'center'>Hello %s %s</h2>" % (first_name, last_name)
msg+= "</body>"
msg+= "</html>"

print "Content-type:text/html"
print "Content-length: " + str(len(msg)) + "\r\n"
print msg
