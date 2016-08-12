# -*- coding: utf-8 -*-
"""
Created on Wed Aug 10 14:13:37 2016

@author: ferdinand

This script transform harish grid multiple txt files to input for Telang processing code

the outputted file is a txt file RRRCCC / time_interval / datetime / count

"""
import sys

gridRes = sys.argv[1]

#gridRes = 50 # grid resolution


#%% ------------- Write HARISH_GRID_HOUR coordinates file -----------------
coord_input = open("/home/ferdinand/Documents/NYU/Data/harish_grid_h/gridCoordinates_" +str(gridRes) + ".txt")
coord_output = open("/home/ferdinand/Documents/NYU/Data/telang/coord_grid_"+ str(gridRes) + ".csv", "w")
coord_output.write("cell_id,longitude,latitude\n")

# Output file has format: RRRCCC / longitude / latitude
# row index RRR increases with latitude
# column index CCC increases with longitude

nxny = coord_input.readline().split(" ")

nx = int(nxny[0])
ny = int(nxny[1])

## SPATIAL ROW ORDER -- CAUTION DO NOT GET CONFUSED
# i designates the index of spatial ROWS --> increases with latitude
# j designates the index of spatial COLUMNS --> increases with longitude

for i in range(ny):
    for j in range(nx):
        cell_id = '{:03}'.format(i) + '{:03}'.format(j)        
        longilati = coord_input.readline().split(" ")
        coord_output.write(str(cell_id) + "," + longilati[0] + "," + longilati[1])

coord_output.close()

#%%-------------- Write HARISH_GRID_HOUR counts file -----------------------

nhours=24
ndays=31

# ------ hourly aggregation -------
ctgrid1110_h = open("/home/ferdinand/Documents/NYU/Data/telang/ctgrid1110_h_"+str(gridRes)+".csv", "w")
ctgrid1110_h.write("cell_id,time_interval,datetime,count\n")

# Output file has format: RRRCCC / time_interval / datetime / count
# time_interval: index of the hour with Oct 1st 2011 midnight has index 0, Oct 1st 2011 1am has index 1...
# datetime in format YYYY-MM-DD-HH

# Maybe computational complexity could be cut by rounding the floats of density? Do NOT round to closest integer because values are around 0 - 10

time_interval = 0

for d in range(1,ndays+1):
    
    for h in range(nhours):
        
        datetime = '2011-10-' + '{:02}'.format(d) + '-' + '{:02}'.format(h)
        count = open('/home/ferdinand/Documents/NYU/Data/harish_grid_h/pick-density_'+str(gridRes)+ '/density-2011-10-' + str(d) + '-' + str(h) + '.txt')      
        
        # SPATIAL ROW MAJOR ORDER
        for i in range(ny):
            for j in range(nx):
                cell_id = '{:03}'.format(i) + '{:03}'.format(j)        
                ct = float(count.readline().replace("\n",""))
                ctgrid1110_h.write(str(cell_id) + ',' + str(time_interval) + ',' + datetime + ',' + str(ct) +'\n')
        
        time_interval+=1
        
ctgrid1110_h.close()
