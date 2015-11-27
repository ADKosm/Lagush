#!/usr/bin/python

import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
first_name = form.getvalue('first_name')
last_name  = form.getvalue('last_name')

msg = """
<html>
<head>
    <title> Post </title>
</head>
<body>

"""

msg += "<h2 align = 'center'>Hello %s %s</h2>" % (first_name, last_name)

msg += """

<form action="/post.cgi" method="post">
First Name: <input type="text" name="first_name"><br />
Last Name: <input type="text" name="last_name" />

<input type="submit" value="Submit" />
</form>

</body>
</html>
"""

print "Content-type:text/html"
print "Content-length: " + str(len(msg)) + "\r\n"
print msg
