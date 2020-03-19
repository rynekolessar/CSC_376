#	symMath.py
#
#	Version 1a
#	2020 March 3
#	Joseph Phillips
import	signal

signal.signal(signal.SIGINT, signal.SIG_IGN)

from sympy import *
x, y, z = symbols('x y z')

while True :
  expression = input(">>> ")

  if  expression == "quit()" :
    break

  try :
    print(eval(expression))
  except SyntaxError :
    print("Syntax error")
