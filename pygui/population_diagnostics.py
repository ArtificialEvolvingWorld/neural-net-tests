import os

from PyQt4 import uic, QtCore, QtGui

import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'population_diagnostics.ui'))

from pygui.util import fill_placeholder


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

        self.ui.num_organisms.setText(str(num_organisms))
        self.ui.num_species.setText(str(num_species))
        self.ui.best_fitness.setText(str(best_fitness))

    def setup_figure(self):
        bg = self.palette().window().color()
        color = (bg.redF(), bg.greenF(), bg.blueF())
        self.figure = plt.figure(edgecolor=color, facecolor=color)
        self.canvas = FigureCanvas(self.figure)
        toolbar = NavigationToolbar(self.canvas, self)
        toolbar.pan()
        toolbar.hide()

        fill_placeholder(self.ui.plot_holder, self.canvas)

    def make_plot(self):
        self.figure.clear()

        axes = self.figure.add_subplot(1,1,1)
        axes.hist([org.fitness
                   for spec in self.population.species
                   for org in spec.organisms])
        axes.set_xlabel('Fitness')
        axes.set_ylabel('Count')
        self.figure.tight_layout()

        self.canvas.draw()
