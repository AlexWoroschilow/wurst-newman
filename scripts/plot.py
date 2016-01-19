#!/usr/bin/python3
# import csv
# import numpy as np
# import matplotlib.pyplot as plt
#
# import numpy as np
# data=np.genfromtxt('../aacid_vs_salami.csv', skip_header=1) #suppose it is in the current working directory
# plt.plot(data)
# plt.show()

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter

# structure a;structure b;tm_score aacid;tm_score salami
data = pd.read_csv('../aacid_vs_salami.csv', sep=';')
data_length = len(data)

weights_aacid = np.ones_like(data['tm_score aacid']) / data_length

fig = plt.figure()
ax1 = fig.add_subplot(111)
ax1.hist(data['tm_score aacid'], color='green', histtype="step", weights=weights_aacid)
ax1.set_xlim(0.4, 1)

plt.grid(True)
plt.title("Aacid vs Salami")
plt.xlabel("aacid tm score")
plt.ylabel("Percentage")
plt.show()
