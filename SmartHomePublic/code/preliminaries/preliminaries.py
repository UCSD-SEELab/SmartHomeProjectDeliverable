'''
BSD 3-Clause License

Copyright (c) 2018, The Regents of the University of California.
All rights reserved.

Created by Anthony Thomas
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

from __future__ import print_function
import os
import re
import sys
import json
import time
import datetime
import shutil
import tempfile
import traceback
import matplotlib
import collections
import datetime
import math
import errno

import numpy as np
import numpy.linalg as alg
import pandas as pd

import matplotlib.pyplot as plt
import pylab as pylab

from sklearn.neural_network import MLPClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import confusion_matrix

from collections import defaultdict
import ast

from dateutil import parser
import tensorflow as tf
from tensorflow.python.framework import tensor_util

np.random.seed(19382)

LABEL_ENCODING = {
    "Clearing Table": 0,
    "Cooking": 1,
    "Drinking Tea": 2,
    "Eating": 3,
    "Making Tea": 4,
    "Prepping Food": 5,
    "Setting Table": 6,
    "Watching TV": 7
}

LABEL_ENCODING2NAME = {
    0: "Clearing Table",
    1: "Cooking",
    2: "Drinking Tea",
    3: "Eating",
    4: "Making Tea",
    5: "Prepping Food",
    6: "Setting Table",
    7: "Watching TV"
}
