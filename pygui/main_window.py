import os
from PyQt4 import uic, QtCore
from PyQt4.QtGui import *
import itertools
import sys
sys.path.append(os.path.join(os.path.dirname(__file__),'../lib'))
import pyneat

(Ui_MainWindow, QMainWindow) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'main_window.ui'))

class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        # set up callbacks
        self.ui.advance_one_gen.clicked.connect(self.advance_one_gen)
        self.ui.advance_ten_gen.clicked.connect(self.advance_ten_gen)
        self.ui.advance_n_gen.clicked.connect(self.advance_n_gen)
        self.ui.mutation_rate.valueChanged.connect(self.mutation_rate_changed)

        # set up data model
        standard_model = QStandardItemModel()
        self.generation_parent = QStandardItem("Generations")
        standard_model.invisibleRootItem().appendRow(self.generation_parent)

        # register model with the ui tree view
        self.ui.tree_view.setModel(standard_model)

        # initialize pyneat neat_xor example
        self.prob = pyneat.Probabilities()
        self.seed = pyneat.Genome.ConnectedSeed(2,1)
        self.rng = pyneat.RNG_MersenneTwister(1)
        self.prob.new_connection_is_recurrent = 0
        self.generations = [pyneat.Population(self.seed, self.rng, self.prob)]


    def advance_one_gen(self):
        self.generations.append(self.generations[-1].Reproduce(xor))
        item = QStandardItem("Gen. {}".format(len(self.generations)-1))
        self.generation_parent.appendRow(item)

    def advance_ten_gen(self):
        print('Advance ten generation')

    def advance_n_gen(self):
        n = self.ui.num_gens.value()
        print('Advance {} generation'.format(n))

    def mutation_rate_changed(self,value):
        print('Mutation rate changed to {}'.format(value))

def xor(net):
    if net.num_connections == 0:
        return 0.0

    error = 0.0
    for i1,i2 in itertools.product([0,1],[0,1]):
        correct = i1 ^ i2
        val = net.evaluate([i1,i2])[0]
        error += pow(val-correct, 2)
    return pow(4.0 - error, 2)
