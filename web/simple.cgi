#!/usr/bin/python3

a = """
<html>
<head>Simple CGI</head>
<body>
This is a simple CGI script based on python
</body>
</html>
"""

print("Content-type:text/html")
print("Content-length: " + str(len(a)) + "\r\n")
print(a)
