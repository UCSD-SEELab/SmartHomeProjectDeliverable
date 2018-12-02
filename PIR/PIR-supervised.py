
# coding: utf-8

# In[1]:


from sklearn.cluster import KMeans
from sklearn.mixture import GaussianMixture
from sklearn.decomposition import PCA, FastICA
from sklearn.model_selection import GridSearchCV
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import AdaBoostClassifier, RandomForestClassifier
from sklearn.svm import SVC
from sklearn.naive_bayes import GaussianNB
from sklearn.neighbors import KNeighborsClassifier
from sklearn.cross_validation import train_test_split
from sklearn.metrics import f1_score
import numpy as np
import pandas as pd
import json
import pickle


# In[2]:


def sum_pir(data):
    new_data = []
    for r in range(len(data)):
        new_entry = [0,0,0,0,0]
        for c in range(len(data[r])):
            new_entry[c%5] += data[r][c]
        new_data.append(new_entry)
    return new_data

with open("data_2_28.txt", "r") as file:
    lines = file.readlines()
print len(lines)
features = []
label = []
y_d = []
y_v = []
for i in lines:
    tmp = json.loads(i)
    if int(tmp["v"])==0:
        features.append(tmp["values"])
        label.append(tmp["y"])

features = pd.DataFrame(features)
label = pd.Series(label)
print len(label)


# In[81]:


ica = PCA(n_components=5).fit(features)
print ica.explained_variance_ratio_
X = ica.transform(features)
# X = features
y = label
X_train,X_test,y_train,y_test = train_test_split(X,y,test_size=0.1)


# In[82]:


# Decision Tree
param_grid = [{'max_depth':range(1,20),'min_samples_leaf':range(1,20)}]
clf = GridSearchCV(DecisionTreeClassifier(),param_grid).fit(X_train,y_train)
clusterer = clf.best_estimator_ 
print clusterer.score(X_test,y_test)


# In[78]:


# Gaussian NB
clf = GaussianNB().fit(X_train,y_train)
print clf.score(X_test,y_test)


# In[47]:


# ensemble
clf = RandomForestClassifier().fit(X_train,y_train)
print clf.score(X_test,y_test)


# In[37]:


# KNeighbors
param_grid = [{'n_neighbors':range(1,20)}]
clf = GridSearchCV(KNeighborsClassifier(),param_grid).fit(X_train,y_train)
clusterer = clf.best_estimator_ 
print clusterer.score(X_test,y_test)


# In[ ]:


statistics = np.around(sum_pir(ica.inverse_transform(clusterer.means_)),1)
for i in statistics:
    print i


# In[85]:


labels = ['window','refrig','trash','oven','cook','sink','bed','onbed','wall']


# In[83]:


with open("DecisionTree","wb") as sf1:
    pickle.dump(clf, sf1)


# In[84]:


with open("decomposition","wb") as sf2:
    pickle.dump(ica, sf2)


# In[86]:


with open("labels","wb") as sf3:
    pickle.dump(labels, sf3)

