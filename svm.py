import numpy as np
X = np.array([
   [ 0.0,  1.0],
   [ 2.0,  3.0],
   [ 4.0, -1.0],
   [-5.0,  4.0],
   [-2.0, -1.0]])

Y = np.array([
     1.0,
     1.0,
    -1.0,
     1.0,
    -1.0])

from sklearn.svm import SVC
clf = SVC(kernel='linear')
clf.fit(X,Y)
print(clf.coef_)
