# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import turtle as t
import sys



def main():
    blocklist = []
    # print(sys.argv[1])
    file = open(sys.argv[1], 'r')
    bound = 0
    while True:
        line = file.readline()
        if not line: break
        if "Boundary" in line:
            temp = line.split()
            bound = int(temp[1])
        if "sb" not in line: pass
        else:
            position = parser(line)
            blocklist.append(position)
    file.close()
    draw(blocklist, bound)
    
def square(x, y, w, h, index):
    t.goto(x+3, y)
    t.pendown()
    t.write(index, font=("Arial", 6, "normal"))
    t.penup()
    t.goto(x, y)
    t.pendown()
    t.setheading(0) 
    for i in range(2):
        t.forward(w)
        t.left(90) #向右轉90度
        t.forward(h)
        t.left(90)
    t.penup()
    

def boundary(bound):
    t.pendown()
    t.setheading(0)
    for i in range(2):
        t.forward(bound)
        t.left(90) #向右轉90度
        t.forward(bound)
        t.left(90)
    t.penup()
    t.pensize(1)
    t.color('black')
    
def draw(blist, bd):
    t.screensize(1200, 1200)
    t.hideturtle()
    t.speed('fastest')
    t.pensize(2)
    t.color('red')
    boundary(bd)
    for index, block in enumerate(blist):
        square(block[0], block[1], block[3], block[4], index)
    t.goto(0, bd+1)
    title = "n" + str(len(blist)) + " floorplan"
    t.pensize(5)
    t.write(title, font=("Arial", 15, "normal"))
    t.exitonclick()
    t.mainloop()


           
def parser(line):
    str2 = line.split(" ")
    elementlist = str2[1:]
    finallist = [int(x) for x in elementlist]
    return finallist

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Wrong Input!")
    else:
        # filename = sys.argv[1]
        # main(filename)
        # main(sys.argv[1])
        main()