# coding: utf8
import codecs
ir = 0

#f = open('File.txt', 'r')#.read()
f = codecs.open("File.txt", "r", "utf-8")

for line in f:

	if ir != 0:

		print(line)
		print(len(line))
	ir += 1
f.close()
print("")
input("press Enter to exit:")
