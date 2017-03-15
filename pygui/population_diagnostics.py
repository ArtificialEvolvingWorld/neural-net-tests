import os

from PyQt4 import uic, QtCore, QtGui

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'population_diagnostics.ui'))

from pygui.util import fill_placeholder
import TestCases


class PopulationDiagnostics(QWidget):
    def __init__(self, population, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

        self.population = population
        self.load_values()
        self.setup_figure()
        self.make_plot()

    def load_values(self):
        num_species = len(self.population.species)
        num_organisms = sum(len(spec.organisms) for spec in self.population.species)
        best_fitness = max(org.fitness
                           for spec in self.population.species
                           for org in spec.organisms)
        total_adj_fitness = sum(org.adj_fitness
                                for spec in self.population.species
                                for org in spec.organisms)

        self.ui.num_organisms.setText(str(num_organisms))
        self.ui.num_species.setText(str(num_species))
        self.ui.best_fitness.setText(str(best_fitness))
        self.ui.total_adj_fitness.setText(str(total_adj_fitness))

    def setup_figure(self):
        bg = self.palette().window().color()
        color = (bg.redF(), bg.greenF(), bg.blueF())
        self.figure = plt.figure(edgecolor=color, facecolor=color)
        self.canvas = FigureCanvas(self.figure)
        toolbar = NavigationToolbar(self.canvas, self)
        toolbar.pan()
        toolbar.hide()

        fill_placeholder(self.ui.plot_holder, self.canvas)

    # def make_plot(self):
    #     self.figure.clear()

    #     axes = self.figure.add_subplot(1,1,1)
    #     axes.hist([org.fitness
    #                for spec in self.population.species
    #                for org in spec.organisms])
    #     axes.set_xlabel('Fitness')
    #     axes.set_ylabel('Count')
    #     self.figure.tight_layout()

    #     self.canvas.draw()

    def make_plot(self):
        best_fitness = max(org.fitness
                           for spec in self.population.species
                           for org in spec.organisms)


        self.figure.clear()
        axes = self.figure.add_subplot(1,1,1)
        data = TestCases.regression_1d_data
        domain = np.arange(min(data),max(data)+0.1,0.1);



        for spec in self.population.species:
            best_in_spec = None
            for org in spec.organisms:
                #if best_in_spec is not None and org.fitness > best_in_spec.fitness:
                #    best_in_spec = org
                if org.fitness == best_fitness:
                    best_net = org.network
                else:
                    net = org.network
                    results = []
                    for x in domain:
                        results.append(net.evaluate([x])[0])
                    axes.plot(domain,results,color='#eeeeee', linestyle='-')
        results = []
        for x in domain:
            results.append(best_net.evaluate([x])[0])
        axes.plot(domain,results,'k-')
            # if best_in_spec is not None and best_in_spec.fitness != best_fitness:
            #     results = []
            #     net = best_in_spec.network
            #     for x in domain:
            #         results.append(net.evaluate([x])[0])
            #     axes.plot(domain,results,color='#bbbbbb',linestyle='-')


        axes.scatter(data,TestCases.regression_1d_label,marker='x',color='red')
        plt.ylim(min(TestCases.regression_1d_label)*0.5,max(TestCases.regression_1d_label)*1.1)
        plt.xlim(min(data)*0.5,max(data)*1.2)

        self.figure.tight_layout()

        self.canvas.draw()
