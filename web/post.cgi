#!/usr/bin/python

import cgi, cgitb 

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
#first_name = form.getvalue('first_name')
#last_name  = form.getvalue('last_name')

msg = """
<html>
<head>
    <title> Post </title>
</head>
<body>

<form action="post.cgi" method="post">
    Your name: <input type="text" name="nam"\><br>
    Your surname: <input type="text" name="surnam"\><br>
    <input type="submit" value="Yo">
</form>

</body>
</html>
"""

print "Content-type:text/html"
print "Content-length: " + str(len(msg)) + "\r\n"
print msg
