import mapgenerator
import random
import math

system_count = 5

min_system_distance = 500000

system_radius_mean = 50000

system_radius_deviation = 10000

universe_radius = 500000000

class Point:
    def __init__(self, x,y):
        self.x = x
        self.y = y
    
    def distanceSquared(self, point):
        dx = self.x - point.x
        dy = self.y - point.y
        return dx*dx+dy*dy
    
    def distance(self, point):
        return math.sqrt(self.distanceSquared(point))

def nextPosition():
    return Point(random.normalvariate(universe_radius*0.5, universe_radius*0.6),random.normalvariate(universe_radius*0.5, universe_radius*0.6))

def generateRandomSystem():
    position = nextPosition()
    system_radius = random.normalvariate(system_radius_mean, system_radius_deviation)
    mapgenerator.setPosition(position.x, position.y)
    mapgenerator.setRadius(system_radius)
    mapgenerator.nextStarSystem()

for i in range(system_count):
    generateRandomSystem()