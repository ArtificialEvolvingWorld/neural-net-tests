import os

from PyQt4 import uic, QtCore, QtGui

import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'species_diagnostics.ui'))

from pygui.util import fill_placeholder


class SpeciesDiagnostics(QWidget):
    def __init__(self, species, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

        self.species = species
        self.load_values()
        self.setup_figure()
        self.make_plot()

    def load_values(self):
        num_organisms = len(self.species.organisms)
        current_best = max(org.fitness for org in self.species.organisms)
        total_adj_fitness = sum(org.adj_fitness for org in self.species.organisms)
        self.ui.num_organisms.setText(str(num_organisms))
        self.ui.best_historical_fitness.setText(str(self.species.best_fitness))
        self.ui.age.setText(str(self.species.age))
        self.ui.best_fitness.setText(str(current_best))
        self.ui.adj_fitness.setText(str(total_adj_fitness))

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
        axes.hist([org.fitness for org in self.species.organisms])
        axes.set_xlabel('Fitness')
        axes.set_ylabel('Count')
        self.figure.tight_layout()

        self.canvas.draw()
