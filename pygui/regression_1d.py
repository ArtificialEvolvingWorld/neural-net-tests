import os
import numpy as np

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget_diag, QWidget_diag) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'regression_1d_diagnostics.ui'))

(Ui_Widget_opts, QWidget_opts) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'regression_1d_opts.ui'))

class Regression1DDiagnostics(QWidget_diag):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget_diag()
        self.ui.setupUi(self)
        self.figure = self.ui.matplotlibCanvas.figure

    def update(self, org, opts):
        data = opts['data']
        label = opts['label']

        self.figure.clear()
        axes = self.figure.add_subplot(1,1,1)
        domain = np.arange(min(data),max(data)+0.1,0.1);

        results = []
        for x in domain:
            results.append(org.network.evaluate([x])[0])
        axes.plot(domain,results,'k-')

        axes.scatter(data, label, marker='x', color='red')

        axes.set_ylim(min(label)*0.5, max(label)*1.1)
        axes.set_xlim(min(data)*0.5, max(data)*1.2)

        self.figure.tight_layout()
        self.figure.canvas.draw()



class Regression1DOptions(QWidget_opts):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget_opts()
        self.ui.setupUi(self)

    def options(self):
        return {'data': [1.0,1.5,2.0,2.5,3.0,3.5,4,5,6,7,8],
                'label': [1,1.2,1.4,1.38,1.2,2,5,8,10,10.5,11],
                }

def regression_default_prob(prob):
    prob.new_connection_is_recurrent = 0
    prob.use_compositional_pattern_producing_networks = True
    prob.mutation_prob_toggle_connection = 0.005

    prob.genetic_distance_species_threshold = 3.0
    prob.weight_mutation_small_adjust = 0.01
    prob.genetic_distance_structural = 3
    prob.mutation_prob_add_connection = 0.01
    prob.mutation_prob_add_node = 0.006
