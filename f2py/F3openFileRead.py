# coding: utf8
import codecs
#from codecs import open

#f = open('File.txt', 'r')#.read()
f = codecs.open("File.txt", "r", "utf-8")
for line in f:

	print(line)
	print(len(line)-1)

f.close()
print("•")
input("press Enter to exit:")
