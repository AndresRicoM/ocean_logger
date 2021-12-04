import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import seaborn as sns; sns.set()  # for plot styling
from matplotlib import gridspec
from matplotlib import rc,rcParams

test_file_name = 'charles_river.txt'
test_data_path = '/Users/AndresRico/Desktop/working/ocean_logger/data/'
plot_title = 'River Intermitent Experimental Log'

# millis, ph, pressure, temperature, depth, altitude

print('Importing Files')

raw_data = np.genfromtxt(test_data_path + test_file_name, delimiter = ',', invalid_raise=False)
print('Getting Data Stats...')

print('Shape of input data is: ' , raw_data.shape)
print('Size of input data is: ' , raw_data.size)

print('Generating Data Visualization')

#plt.style.use('dark_background')
raw_data[:,0] = raw_data[:,0] / 60000 #Modify Time Axis
dot_size = 10

input_data = raw_data
#raw_data
#raw_data[np.where(raw_data[:,0] > 3523)[0][0] : np.where(raw_data[:,0] < 3545)[0][-1] , :] #Pool
#raw_data[np.where(raw_data[:,0] > 3303)[0][0] : np.where(raw_data[:,0] < 3309)[0][-1] , :] #Ocean1
#raw_data[np.where(raw_data[:,0] > 4627)[0][0] : np.where(raw_data[:,0] < 4720)[0][-1] , :] #Full Ocean
input_data[:,0] = input_data[:,0] - input_data[0,0]

gs = gridspec.GridSpec(3,1)
fig = plt.figure()
fig.suptitle(plot_title, size=30)
# activate latex text rendering
rc('text', usetex=True)
rc('axes', linewidth=2)
rc('font', weight='bold')
rcParams['text.latex.preamble'] = [r'\usepackage{sfmath} \boldmath']

"""
ax = fig.add_subplot(gs[0])
ax.plot(input_data[:,0], input_data[:,1] , color='mediumaquamarine')
ax.set_ylabel(r'\textbf{pH}', size =14)
ax.set(xticklabels=[])
plt.grid(color = 'gray', linestyle = '--', linewidth = 0.5)
"""


ax = fig.add_subplot(gs[0])
ax.plot(input_data[:,0], input_data[:,2], color='teal')
ax.set_ylabel(r'\textbf{Temperature (C)}', size =14)
ax.set(xticklabels=[])
plt.grid(color = 'gray', linestyle = '--', linewidth = 0.5)


"""
ax = fig.add_subplot(gs[2])
ax.plot(input_data[:,0], input_data[:,3], color='darkturquoise')
ax.set_ylabel(r'Pressure (mbar)', size =14)
ax.set(xticklabels=[])
"""


ax = fig.add_subplot(gs[1])
ax.plot(input_data[:,0], input_data[:,4], color='cadetblue')
ax.set_ylabel(r'\textbf{Depth (m)}', size =14)
ax.set(xticklabels=[])
plt.grid(color = 'gray', linestyle = '--', linewidth = 0.5)


"""
ax = fig.add_subplot(gs[2])
ax.plot(input_data[:,0], input_data[:,5], color='powderblue')
ax.set_ylabel(r'Altitude (m)', size =14)
ax.set(xticklabels=[])
plt.grid(color = 'gray', linestyle = '--', linewidth = 0.5)
"""

ax = fig.add_subplot(gs[2])
ax.plot(input_data[:,0], input_data[:,6], color='skyblue')
ax.set_ylabel(r'\textbf{Light (raw)}', size =14)
ax.set_xlabel(r'\textbf{Time (min)}', size =14)

plt.grid(color = 'gray', linestyle = '--', linewidth = 0.5)

fig.align_ylabels()


"""
gs = gridspec.GridSpec(2,1)
fig = plt.figure()
dot_size = 3

ax = fig.add_subplot(gs[0])
ax.plot(rm[:,0],rm[:,5], 'y')
ax.plot(rm[:,0],rm[:,2], 'g')
ax.plot(rm[:,0],rm[:,3], 'b')
ax.plot(rm[:,0],rm[:,1],'r')

ax.set_ylabel(r'Piezo Array', size =16)

ax = fig.add_subplot(gs[1])
ax.plot(rm[:,0],rm[:,7],'r')
ax.plot(rm[:,0],rm[:,8], 'g')
ax.plot(rm[:,0],rm[:,9], 'b')
#ax.plot(rm[:,0],rm[:,23], 'y')
ax.set_ylabel(r'Pressure', size =16)
"""

plt.show()
