import numpy as np
import matplotlib.pyplot as plt

side = 10

# draw a square as a function of theta
t = np.linspace(0, 2*np.pi, 1000)
p_4 = np.pi/4

r = np.piecewise(t, [
    t <= p_4, (t > p_4) & (t <= 3*p_4), (t > 3*p_4) & (t <= 5*p_4), (t > 5*p_4) & (t <= 7*p_4), t > 7*p_4
],
    [
    lambda t: side/np.cos(t), lambda t: side/np.sin(t), lambda t: -side/np.cos(t), lambda t: -side/np.sin(t), lambda t: side/np.cos(t)
])
# r = side/np.cos(t)
fig = plt.figure()
ax = fig.add_subplot(polar=True)
# r = np.abs(r)
ax.plot(t, r)

plt.show()
