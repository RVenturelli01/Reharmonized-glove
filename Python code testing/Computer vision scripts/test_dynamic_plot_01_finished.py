import numpy as np
import matplotlib.pyplot as plt
import random

# Initialize the figure
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')


while True:
    ax.cla()
    ax.set_xlim3d(0, 1)
    ax.set_ylim3d(0, 1)
    ax.set_zlim3d(0, 1)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    
    # Genera tre variabili float casuali tra 0 e 1
    x = random.random()
    y = random.random()
    z = random.random()

    ax.scatter(x,y,z, c='r', marker='o')

    plt.pause(0.1)
    plt.draw()
    
