# -*- coding: utf-8 -*-
"""
Created on Wed Jan 27 16:35:23 2021

@author: user
"""

# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import turtle as t
import sys



def main():
    v_list = []
    file = open(sys.argv[1], 'r')
    lines = file.readlines()
    num_h = int(lines[0])
    num_v = int(lines[1])
    ## 2 - > h, 4 -> v
    h_list = [float(element) for element in lines[2][:len(lines[2])-2].split(" ")]
    v_list = [float(element) for element in lines[4][:len(lines[4])-1].split(" ")]
    print(len(h_list))
    print(len(v_list))
    file.close();
    draw(h_list, v_list, num_h, num_v)

    
    
def draw(h, v, numh, numv):
    screen = t.Screen()
    screen.bgcolor('black') 
    screen.setup(1920, 1080) 
    # t.screensize(1200, 1200)
    t.speed('fastest')
    t.tracer(False)
    t.pensize(2)
    t.penup()
    t.goto(-500, -500)
    t.pendown()
    current_row = 0
    edge_length = 15
    for index, val in enumerate(h):
        row = index // (numh - 1)
        # col = index % (numh -1)
        if current_row < row:
            current_row = row
            t.penup()
            t.goto(-500, -500 + row * edge_length)
            t.pendown()
        # print(col, ", ", row)
        t.setheading(0)
        if val == 0:
            t.color("white")
        elif val > 0 and val <= 0.25:
            t.color("DodgerBlue")
        elif val > 0.25 and val <= 0.5:
            t.color("SkyBlue") # SkyBlue
        elif val > 0.5 and val <= 0.75:
            t.color("green")
        elif val > 0.75 and val <= 1:
            t.color("yellow")
        else:
            t.color("red")
        # t.penup()
        # t.forward(1)
        # t.pendown()
        t.forward(edge_length)
    current_col = 0
    t.penup()
    t.goto(-500, -500)
    t.pendown()
    # draw_count = 0
    for index, val in enumerate(v):
        col = index // (numv - 1)
        # col = index % (numh -1)
        if current_col < col:
            # print(draw_count)
            # draw_count = 0
            current_col = col
            t.penup()
            t.goto(-500 + col * edge_length, -500)
            t.pendown()
        # print(col, ", ", row)
        # draw_count += 1
        # print(val)
        t.setheading(90)
        if val == 0:
            t.color("white")
        elif val > 0 and val <= 0.25:
            t.color("DodgerBlue")
        elif val > 0.25 and val <= 0.5:
            t.color("SkyBlue") # SkyBlue
        elif val > 0.5 and val <= 0.75:
            t.color("green")
        elif val > 0.75 and val <= 1:
            t.color("yellow")
        else:
            t.color("red")
        # t.penup()
        # t.forward(1)
        # t.pendown()
        t.forward(edge_length)
    t.penup()
    t.forward(1)
    t.pendown()
    # print(draw_count)
    screen.exitonclick()
    screen.mainloop()
        


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Wrong Input!")
    else:
        # filename = sys.argv[1]
        # main(filename)
        # main(sys.argv[1])
        main()