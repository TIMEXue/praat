echo script
# Paul Boersma, 9 September 2017

asserterror Unknown variable:'newline$'« hg
hg

a = 1
asserterror Expected the end of the formula, but found a numeric variable:
a = 5a

asserterror The variable b does not exist. You can modify only existing variables.
b += 5

printline OK
