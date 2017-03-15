import itertools
import os
import sys

import numpy as np
from PyQt4 import uic, QtCore, QtGui

sys.path.append(os.path.join(os.path.dirname(__file__),'../lib'))
import pyneat
import TestCases
import IPython

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

prob_spinboxes = ['population_size',
                  'min_size_for_champion',
                  'culling_ratio',
                  'stale_species_num_generations',
                  'stale_species_penalty',
                  'matching_gene_choose_mother',
                  'keep_non_matching_mother_gene',
                  'keep_non_matching_father_gene',
                  'mutation_prob_adjust_weights',
                  'weight_mutation_is_severe',
                  'weight_mutation_small_adjust',
                  'weight_mutation_reset_range',
                  'mutation_prob_add_connection',
                  'new_connection_is_recurrent',
                  'mutation_prob_add_node',
                  'mutation_prob_reenable_connection',
                  'mutation_prob_toggle_connection',
                  'genetic_distance_structural',
                  'genetic_distance_weights',
                  'genetic_distance_species_threshold',
]

prob_checkboxes = ['keep_empty_species',
                   'species_representative_from_previous_gen',
]

class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        # set up callbacks
        self.ui.advance_one_gen.clicked.connect(self.advance_one_gen)
        self.ui.advance_ten_gen.clicked.connect(self.advance_ten_gen)
        self.ui.advance_n_gen.clicked.connect(self.advance_n_gen)

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
        self.seed = pyneat.Genome.ConnectedSeed(1,1)
        self.rng = pyneat.RNG_MersenneTwister()
        #self.prob.population_size = 1000
        self.prob.new_connection_is_recurrent = 0
        self.prob.use_compositional_pattern_producing_networks = True
        self.prob.mutation_prob_toggle_connection = 0.005

        self.prob.genetic_distance_species_threshold = 3.0
        self.prob.weight_mutation_small_adjust = 0.01
        self.prob.genetic_distance_structural = 3
        #self.prob.genetic_distance_weights = 0.4
        self.prob.mutation_prob_add_connection = 0.01
        self.prob.mutation_prob_add_node = 0.006



        #self.prob.keep_empty_species = True
        #self.prob.stale_species_penalty = 1.0
        #self.prob.stale_species_num_generations = 100

        #data = [0.0,1.1,2.4,3.9,4.2,5.0,6.5]
        #label = [x*x for x in data]

        #data = np.arange(0,np.pi,0.2)
        #label = np.sin(data)

        data = [1.0,1.5,2.0,2.5,3.0,3.5,4,5,6,7,8]
        label = [1,1.2,1.4,1.38,1.2,2,5,8,10,10.5,11]



        TestCases.regression_1d_data = data
        TestCases.regression_1d_label = label

        self.fitness_func = TestCases.regression_1d_fitness(data,label)
        self.generations = [pyneat.Population(self.seed, self.rng, self.prob)]
        self.generations[-1].Evaluate(self.fitness_func)
        self.add_to_treeview(self.generations[-1], 0)

        self._load_probabilities_from_cpp()
        self._setup_probabilities_callbacks()

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

    def _load_probabilities_from_cpp(self):
        for name in prob_spinboxes:
            spinbox = getattr(self.ui,name)
            spinbox.setValue(getattr(self.prob,name))

        for name in prob_checkboxes:
            checkbox = getattr(self.ui,name)
            checkbox.setChecked(getattr(self.prob,name))


    def _setup_probabilities_callbacks(self):
        for name in prob_spinboxes:
            spinbox = getattr(self.ui,name)
            spinbox.valueChanged.connect(self._load_probabilities_to_cpp)

        for name in prob_checkboxes:
            checkbox = getattr(self.ui,name)
            checkbox.clicked.connect(self._load_probabilities_to_cpp)


    def _load_probabilities_to_cpp(self):
        for name in prob_spinboxes:
            spinbox = getattr(self.ui,name)
            setattr(self.prob, name, spinbox.value())

        for name in prob_checkboxes:
            checkbox = getattr(self.ui,name)
            setattr(self.prob, name, bool(checkbox.checkState()))
