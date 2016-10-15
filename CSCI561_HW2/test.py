#!/usr/bin/env python
# coding=utf-8

import os

for i in xrange(1,11):
	print("--Test Case #{0}--".format(i))
	os.system('cp ./TestCase/Test{0}/input.txt ./input.txt'.format(i))
	os.system('time ./a.out')
	print("")
	#os.system('diff ./output.txt ./Test{0}/output.txt'.format(i))
	os.system('cp ./output.txt ./TestCase/Test{0}/output.txt'.format(i))

os.system('rm input.txt output.txt')