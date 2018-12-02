'''
BSD 3-Clause License

Copyright (c) 2018, The Regents of the University of California.
All rights reserved.

Created by Yunhui Guo
SEE Lab
University of California, San Diego

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
'''

import sys
sys.path.append('../')

import scipy.stats as stats
from preliminaries.preliminaries import *

if __name__=="__main__":   
    with open("../../temp/sensors.txt") as fh:
        sensors = eval(fh.read())

    clf = "DecisionTree"
    
    yunhui_data = pd.read_hdf("../../temp/data_processed.h5", "yunhui")
    anthony_data = pd.read_hdf("../../temp/data_processed.h5", "anthony")

    train_data = anthony_data
    test_data = yunhui_data

    train_X  = train_data.drop(['label'], axis=1).values[:-300,:]

    train_y = train_data['label'].values[:-300]

    validation_split = np.random.binomial(1, 0.80, size=(test_data.shape[0],))
    test_X  = test_data.drop(
        ['label'], axis=1).loc[validation_split == 0,:].values

    test_y = test_data['label'][validation_split == 0].values
    validation_X  = test_data.drop(
        ['label'], axis=1).loc[validation_split == 1,:].values
    validation_y = test_data['label'][validation_split == 1].values

    # count each label
    log_dir = "../output/TreeModels/" + clf + "/"

    try:
        os.makedirs(log_dir)
    except OSError as e:
        if e.errno != errno.EEXIST:
            raise

    if clf == "XGboost":
        try:
            os.makedirs(log_dir + "XGboost/")
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise
        f = log_dir + "/XGboost/results.json"
        model = XGBClassifier()

        model.fit(train_X, train_y)
        print "Train acc: {}".format(model.score(train_X, train_y)) 
        print "Test acc: {}".format(model.score(test_X, test_y)) 

    elif clf == "DecisionTree":
        min_impurity_decrease = [0]
        min_samples_split = [2]

        try:
            os.makedirs(log_dir + "DecisionTree/")
        except OSError as e:
            if e.errno != errno.EEXIST:
                raise
        f = log_dir + "/DecisionTree/results.json"

        for ms in min_samples_split:
            for mi in min_impurity_decrease:
                model = DecisionTreeClassifier(min_samples_split=ms, min_impurity_decrease=mi, random_state=0)
                json_ = {"min_impurity_decrease": mi, "min_samples_split": ms}
                model.fit(train_X, train_y)
                print "Train acc: {}".format(model.score(train_X, train_y)) 
                print "Test acc: {}".format(model.score(test_X, test_y)) 
                tree.export_graphviz(model, out_file='tree.dot')   
