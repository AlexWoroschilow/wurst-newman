#!/usr/bin/python2

import pandas as pd
import numpy as np
import statsmodels.api as sm

data = pd.read_csv('../aacid_vs_salami_1.csv', sep=';')

x = data['tm_score aacid']
y = data['tm_score salami']


def reg_m(y, x):
    ones = np.ones(len(x))
    X = sm.add_constant(np.column_stack((x, ones)))
    results = sm.OLS(y, X).fit()
    return results


print(reg_m(y, x).summary())
