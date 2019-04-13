# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import numpy as np
#%%
file1 = 'A1output'
#time_serial = 6.823
#cal_per = 0.885

speedup_theo = [1 for i in list(range(1,33))]

with open(file1, 'r') as f:
    ptime = [float(line.split()[2]) for line in f.readlines()[:32]]

# =============================================================================
# # For average testing
# speedup_theo = list(range(1,33))
# 
# with open(file1, 'r') as f:
#     ptime1 = [float(line.split()[2]) for line in f.readlines()[:32]]
# with open(file1, 'r') as f:
#     ptime2 = [float(line.split()[2]) for line in f.readlines()[32:64]]
# with open(file1, 'r') as f:
#     ptime3 = [float(line.split()[2]) for line in f.readlines()[64:]]
# 
# ptime = (np.array(ptime1) + np.array(ptime2) + np.array(ptime3)) / 3
# =============================================================================


speedup = [ptime[0] / time for time in ptime]


#%%
plt.figure(figsize=(10, 6))
plt.plot(list(range(1,33)), speedup_theo, 'g--', label='Theoretical')
plt.plot(list(range(1,33)), speedup, '.-', label='Measured')

plt.xlabel('Number of processors', fontsize=12)
plt.ylabel('Speedup', fontsize=12)
plt.title('Scalability Performance (scaled problem size)', fontsize=16)
plt.legend()  # auto-legend with labels above.
plt.savefig('scalability.png')
plt.show()

#%%
'''
with open(file1, 'r') as f:
    reals = [line.split()[1] for 
             line in f.readlines() if 
             line.split() and line.split()[0] == 'real']
with open(file1, 'r') as f:
    users = [line.split()[1] for 
             line in f.readlines() if 
             line.split() and line.split()[0] == 'user']

reals = [float(time.split('m')[0]) * 60 + 
         float(time.split('m')[1][:-1]) for 
         time in reals]
users = [float(time.split('m')[0]) * 60 + 
         float(time.split('m')[1][:-1]) for 
         time in users]
'''
