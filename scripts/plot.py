#!/usr/bin/python3
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# structure a;structure b;tm_score aacid;tm_score salami
data = pd.read_csv('../aacid_vs_salami_1.csv', sep=';')
data_length = len(data)

weights_aacid = np.ones_like(data['tm_score aacid']) / data_length

x = data['tm_score aacid']
y = data['tm_score salami']

fit = np.polyfit(x, y, 1)
fit_fn = np.poly1d(fit)

fig = plt.figure()
ax1 = fig.add_subplot(111)
ax1.plot(x, y, 'yo', x, fit_fn(x), '--k')
# ax1.hist(data['tm_score aacid'], color='green', histtype="step", weights=weights_aacid)
# ax1.hist([data['tm_score aacid'], data['tm_score salami']], histtype="step")
ax1.set_xlim(0.4, 1)

plt.grid(True)
plt.title("aacid vs salami")
plt.xlabel("aacid tm score")
plt.ylabel("percentage")
plt.show()
