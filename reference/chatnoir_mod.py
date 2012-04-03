import os, sys, zlib

orig = '96040008130500'.decode('hex')
mod = '96040008130501'.decode('hex')
assert len(orig) == len(mod)

f = open('chatnoir.swf', 'r+b')
h = f.read(8)
s = f.read()
s = zlib.decompress(s)
p = s.find(orig)
s = s[:p] + mod + s[p+len(orig):]
s = zlib.compress(s)
f.seek(0)
f.truncate(0)
f.write(h)
f.write(s)
f.close()
