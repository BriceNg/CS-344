#!/usr/bin/env python

#Create and Write Random String to First File
f= open('File1.txt', 'w+')															#Create File1.txt
import random
import string
random = ''.join([random.choice(string.ascii_lowercase) for n in xrange(10)])		#Create Random String Of Length = 10
f.write("%s\n" % (random))															#Write String In File1.txt w/ Newline
f.close()																			#Close File

#Create and Write Random String to Second File
f= open('File2.txt', 'w+')															#Create File2.txt
import random
import string
random = ''.join([random.choice(string.ascii_lowercase) for n in xrange(10)])		#Create Random String Of Length = 10
f.write("%s\n" % (random))															#Write String In File2.txt w/ Newline
f.close()																			#Close File

#Create and Write Random String to Third File
f= open('File3.txt', 'w+')															#Create File3.txt
import random
import string
random = ''.join([random.choice(string.ascii_lowercase) for n in xrange(10)])		#Create Random String Of Length = 10
f.write("%s\n" % (random))															#Write String In File3.txt w/ Newline
f.close()																			#Close File

#Print Contents of First File
f= open('File1.txt','r')															#Open File1.txt For Reading
out = f.read()[:-1]																	#Cut Off Newline and Put in out Variable
print(out)																			#Print out
f.close()																			#Close File

#Print Contents of Second File
f= open('File2.txt','r')															#Open File1.txt for Reading
out = f.read()[:-1]																	#Cut Off Newline and Put in out Variable
print(out)																			#Print Variable
f.close()																			#Close File

#Print Contents of Third File
f= open('File3.txt','r')															#Open File1.txt for Reading
out = f.read()[:-1]																	#Cut Off Newline and Put in out Variable
print(out)																			#Print out
f.close()																			#Close File

#Get First Number
from random import *
a = randint(1, 42)																	#Create Random int [1-32] Inclusive
print (a)																			#Print a

#Get Second Number
from random import *
b = randint(1, 42)																	#Create Random int [1-32] Inclusive
print (b)																			#Print b

#Print Product
product = a * b																		#Put Product Of ints Into product
print (product)																		#Print product