# -*- coding: utf-8 -*-
"""
Created on Fri Aug 19 13:34:26 2016

@author: ferdinand

Converts lat long to meters
"""

import math

R_earth = 6371000

# Convert a delta degree longitude to meters
def latToMeter(deg):
    return(2*math.pi*R_earth*deg/360)

# Convert a delta degree latitude to meters
# lat should be indicated in degrees
def longToMeter(deg,lat):
    return(2*math.pi*R_earth*math.cos(lat*math.pi/180)*deg/360)
    

deltaLong = 74.000069-73.977207
deltaLat = 40.764337 - 40.732941

deltaLong = 73.988985 -73.935406
deltaLat =  40.804275 - 40.730833

deltaLong = 74.004893 -73.963089
deltaLat = 40.799114 - 40.741789

deltaX = longToMeter(deltaLong, 40.764337)
deltaY = latToMeter(deltaLat)

theta4 = math.atan(deltaX/deltaY) #output in rad
theta4 = theta4*180/math.pi