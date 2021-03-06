from collections import namedtuple, OrderedDict
import itertools
import os
import sys

import numpy as np
from PyQt4 import uic, QtCore, QtGui

import pyneat

from pygui.cppn_function_tab import CPPNFunctionTab
from pygui.custom_network_seed_tab import CustomNetworkSeedTab

from pygui.population_diagnostics import PopulationDiagnostics
from pygui.species_diagnostics import SpeciesDiagnostics
from pygui.organism_diagnostics import OrganismDiagnostics

from pygui.fitness_functions import fitness_functions
from pygui.util import fill_placeholder


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
                   'use_compositional_pattern_producing_networks',
]

(Ui_MainWindow, QMainWindow) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'main_window.ui'))

class MainWindow(QMainWindow):
    def __init__(self, args, parent=None):
        super().__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self._set_up_fitness_func_dropdown()
        self.options_widget = None
        self.diagnostics_widget = None

        # set up callbacks
        self.ui.advance_one_gen.clicked.connect(self.advance_one_gen)
        self.ui.advance_ten_gen.clicked.connect(self.advance_ten_gen)
        self.ui.advance_n_gen.clicked.connect(self.advance_n_gen)
        self.ui.reset.clicked.connect(self.reset)

        # set up data model
        self.standard_model = QtGui.QStandardItemModel()
        self.standard_model.setColumnCount(2)
        self.standard_model.setHorizontalHeaderLabels(['','Fitness'])

        # register model with the ui tree view
        self.ui.tree_view.setModel(self.standard_model)
        self.ui.tree_view.header().setResizeMode(QtGui.QHeaderView.ResizeToContents)
        self.ui.tree_view.header().setStretchLastSection(False)

        self.custom_network_seed_tab = CustomNetworkSeedTab(self)

        # Set up NEAT
        self._set_up_background_thread()
        self.prob = pyneat.Probabilities()
        self.cppn_tab = CPPNFunctionTab(self.prob, self)
        def init_rng():
            if args.seed is not None:
                self.rng = pyneat.RNG_MersenneTwister(args.seed)
            else:
                self.rng = pyneat.RNG_MersenneTwister()
        self.initialize_rng = init_rng
        self.current_fitness_index = 0
        self.load_fitness_function(fitness_functions[self.current_fitness_index])
        self.queued_generations = 0
        self.ui.queued_label.setText(str(self.queued_generations))

        self._load_probabilities_from_cpp()
        self._setup_probabilities_callbacks()
        self._show_hide_cppn_function_tab()

        # rig tree view selection model callbacks
        self.ui.tree_view.selectionModel().selectionChanged.connect(self.tree_selection)
        self._set_up_diagnostics_by_type()

    def reset(self):
        self.on_select_fitness_func(self.current_fitness_index)

    def tree_selection(self, selected):
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

    def _set_up_fitness_func_dropdown(self):
        for config in fitness_functions:
            self.ui.fitness_func.addItem(config.name)
        self.ui.fitness_func.currentIndexChanged.connect(self.on_select_fitness_func)

    def _set_up_diagnostics_by_type(self):
        layout = QtGui.QVBoxLayout(self.ui.info_box)
        self.ui.info_box.setLayout(layout)

        self.diagnostics_by_type = {}
        for (obj_type, widget_type) in diagnostic_types.items():
            widget = widget_type(self)
            self.diagnostics_by_type[obj_type] = widget
            layout.addWidget(widget)
            widget.hide()

    def show_diagnostics(self, obj):
        if type(obj) in diagnostic_types:
            widget = self.diagnostics_by_type[type(obj)]
            widget.update(obj)

            for other_widget in self.diagnostics_by_type.values():
                if other_widget is widget:
                    other_widget.show()
                else:
                    other_widget.hide()


        if self.diagnostics_widget is not None:
            best_selected = self.get_best_of_selected(obj)
            self.diagnostics_widget.update(best_selected, self.fitness_func_args())

    def get_best_of_selected(self, obj):
        if isinstance(obj, pyneat.Population):
            return max((org for spec in obj.species for org in spec.organisms),
                       key=lambda org:org.fitness)

        elif isinstance(obj, pyneat.Species):
            return max((org for org in obj.organisms),
                       key=lambda org:org.fitness)

        elif isinstance(obj, pyneat.Organism):
            return obj

        else:
            return None


    def _set_up_background_thread(self):
        self.background_thread = None
        self.background_thread_timer = QtCore.QTimer(self)
        self.background_thread_timer.setInterval(1000)
        self.background_thread_timer.timeout.connect(self.check_for_new_generations)
        self.background_thread_timer.start()

    def check_for_new_generations(self):
        if self.background_thread is None:
            return

        while True:
            # Population returned already has evaluation performed.
            gen = self.background_thread.get_next_generation()
            if gen is None:
                break
            self.generations.append(gen)
            self.add_to_treeview(gen, len(self.generations)-1)
            self.queued_generations -= 1
            self.ui.queued_label.setText(str(self.queued_generations))
            self._hide_custom_network_seed_tab()

    def reproduce_in_background(self, n):
        if self.background_thread is None:
            seed = self.custom_network_seed_tab.make_seed()
            pop = pyneat.Population(seed, self.rng, self.prob)
            self.background_thread = pyneat.PopulationBackgroundThread(pop)

        fitness_func = self.fitness_func_generator( **self.fitness_func_args() )
        self.background_thread.perform_reproduction(fitness_func, n)


    def add_next_generation(self, gen):
        fitness_func = self.fitness_func_generator( **self.fitness_func_args() )
        gen.Evaluate(fitness_func)
        self.generations.append(gen)
        self.add_to_treeview(gen, len(self.generations)-1)

    def advance_one_gen(self):
        self.queued_generations += 1
        self.ui.queued_label.setText(str(self.queued_generations))
        self.reproduce_in_background(1)

    def advance_ten_gen(self):
        self.queued_generations += 10
        self.ui.queued_label.setText(str(self.queued_generations))
        self.reproduce_in_background(10)

    def advance_n_gen(self):
        n = self.ui.num_gens.value()
        self.queued_generations += n
        self.ui.queued_label.setText(str(self.queued_generations))
        self.reproduce_in_background(n)

    def on_select_fitness_func(self, selected_index):
        self.current_fitness_index = selected_index
        config = fitness_functions[selected_index]
        self.load_fitness_function(config)

    def load_fitness_function(self, config):
        if config.output_activation_func is None:
            output_activation_func = pyneat.ActivationFunction.Sigmoid
        else:
            output_activation_func = config.output_activation_func

        self.fitness_func_generator = config.generator

        if self.diagnostics_widget is not None:
            fill_placeholder(self.ui.custom_info_box, None)
            self.diagnostics_widget = None

        if config.diagnostics_widget is not None:
            self.diagnostics_widget = config.diagnostics_widget(self)
            fill_placeholder(self.ui.custom_info_box, self.diagnostics_widget)


        if self.options_widget is not None:
            index = self.ui.coltabwidget.indexOf(self.options_widget)
            self.ui.coltabwidget.removeTab(index)
            self.options_widget = None

        if config.options_widget is not None:
            self.options_widget = config.options_widget(self)
            self.ui.coltabwidget.addTab(self.options_widget, config.name)

        self.prob.reset()
        self.initialize_rng()
        if config.default_prob is not None:
            config.default_prob(self.prob)
        self._load_probabilities_from_cpp()

        self.standard_model.clear()
        self.generations = []
        self._show_custom_network_seed_tab(config)
        self.background_thread = None

    def fitness_func_args(self):
        if self.options_widget is None:
            return {}
        else:
            return self.options_widget.options()

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
            spinbox.blockSignals(True)
            spinbox.setValue(getattr(self.prob,name))
            spinbox.blockSignals(False)

        for name in prob_checkboxes:
            checkbox = getattr(self.ui,name)
            checkbox.blockSignals(True)
            checkbox.setChecked(getattr(self.prob,name))
            checkbox.blockSignals(False)

        self._show_hide_cppn_function_tab()
        self.cppn_tab.load_values_from_cpp()


    def _setup_probabilities_callbacks(self):
        for name in prob_spinboxes:
            spinbox = getattr(self.ui,name)
            spinbox.valueChanged.connect(self._on_probability_change)

        for name in prob_checkboxes:
            checkbox = getattr(self.ui,name)
            checkbox.clicked.connect(self._on_probability_change)


    def _on_probability_change(self):
        self._load_probabilities_to_cpp()
        self._show_hide_cppn_function_tab()


    def _show_hide_cppn_function_tab(self):
        show_tab = self.prob.use_compositional_pattern_producing_networks
        index = self.ui.coltabwidget.indexOf(self.cppn_tab)
        if show_tab and index==-1:
            self.ui.coltabwidget.addTab(self.cppn_tab, 'CPPN Functions')
        elif not show_tab and index!=-1:
            self.ui.coltabwidget.removeTab(index)


    def _load_probabilities_to_cpp(self):
        for name in prob_spinboxes:
            spinbox = getattr(self.ui,name)
            setattr(self.prob, name, spinbox.value())

        for name in prob_checkboxes:
            checkbox = getattr(self.ui,name)
            setattr(self.prob, name, bool(checkbox.checkState()))


    def _show_custom_network_seed_tab(self, config):
        self.custom_network_seed_tab.init(config)
        index = self.ui.coltabwidget.indexOf(self.custom_network_seed_tab)
        if index==-1:
            self.ui.coltabwidget.addTab(self.custom_network_seed_tab,
                                        'Initial Seed Network')


    def _hide_custom_network_seed_tab(self):
        index = self.ui.coltabwidget.indexOf(self.custom_network_seed_tab)
        if index!=-1:
            self.ui.coltabwidget.removeTab(index)


