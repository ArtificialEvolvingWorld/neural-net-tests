import itertools
import os
import sys

import numpy as np
from PyQt4 import uic, QtCore, QtGui

sys.path.append(os.path.join(os.path.dirname(__file__),'../lib'))
import pyneat
import TestCases

from pygui.population_diagnostics import PopulationDiagnostics
from pygui.species_diagnostics import SpeciesDiagnostics
from pygui.organism_diagnostics import OrganismDiagnostics
from pygui.util import fill_placeholder

(Ui_MainWindow, QMainWindow) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'main_window.ui'))

diagnostic_types = {pyneat.Population:PopulationDiagnostics,
                    pyneat.Species:SpeciesDiagnostics,
                    pyneat.Organism:OrganismDiagnostics,
                    }

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
        self.standard_model = QtGui.QStandardItemModel()
        self.standard_model.setColumnCount(2)
        self.standard_model.setHorizontalHeaderLabels(['','Fitness'])

        # register model with the ui tree view
        self.ui.tree_view.setModel(self.standard_model)
        self.ui.tree_view.header().setResizeMode(QtGui.QHeaderView.ResizeToContents)
        self.ui.tree_view.header().setStretchLastSection(False)


        # initialize pyneat neat_xor example
        self.prob = pyneat.Probabilities()
        self.seed = pyneat.Genome.ConnectedSeed(2,1)
        self.rng = pyneat.RNG_MersenneTwister(1)
        self.prob.new_connection_is_recurrent = 0
        self.fitness_func = TestCases.xor_fitness
        self.generations = [pyneat.Population(self.seed, self.rng, self.prob)]
        self.generations[-1].Evaluate(self.fitness_func)
        self.add_to_treeview(self.generations[-1], 0)


        # rig tree view selection model callbacks
        QtCore.QObject.connect(self.ui.tree_view.selectionModel(),QtCore.SIGNAL('selectionChanged(QItemSelection,QItemSelection)'),self.tree_selection)


    @QtCore.pyqtSlot('QItemSelection, QItemSelection')
    def tree_selection(self, selected, deselected):
        """ The callback made when the selectionChanged
        signal is emitted from the tree_view class. """
        # currently creates a new widget everytime a seclection
        # is made, which should be changed to something like a
        # dictionary lookup for the needed widget given a
        # selection index (possible key, QIndex from selected.indexes())

        for index in selected.indexes():
            item = self.standard_model.itemFromIndex(index)
            if item.data() is not None:
                self.show_diagnostics(item.data())
                return

    def show_diagnostics(self, obj):
        if type(obj) in diagnostic_types:
            widget_type = diagnostic_types[type(obj)]
            fill_placeholder(self.ui.info_box, widget_type(obj, self))

    def advance_one_gen(self):
        new_generation = self.generations[-1].Reproduce()
        new_generation.Evaluate(self.fitness_func)
        self.generations.append(new_generation)
        self.add_to_treeview(new_generation, len(self.generations)-1)

    def advance_ten_gen(self):
        for i in range(10):
            self.advance_one_gen()

    def advance_n_gen(self):
        n = self.ui.num_gens.value()
        for i in range(n):
            self.advance_one_gen()

    def add_to_treeview(self, gen, gen_number):
        text = "Gen. {}".format(gen_number)

        gen_item = QtGui.QStandardItem(text)
        gen_item.setData(gen)

        species = sorted(gen.species,
                         key=lambda s:s.best_fitness,
                         reverse=True)
        for spec in species:
            spec_item = QtGui.QStandardItem('Species {}'.format(spec.id))
            spec_item.setData(spec)

            organisms = sorted(spec.organisms,
                               key=lambda org:org.fitness,
                               reverse=True)

            for i,org in enumerate(organisms):
                org_item = QtGui.QStandardItem('Org. {}'.format(i))
                org_item.setData(org)
                spec_item.appendRow(org_item)

            gen_item.appendRow(spec_item)

        self.standard_model.appendRow(gen_item)

    def mutation_rate_changed(self,value):
        print('Mutation rate changed to {}'.format(value))
