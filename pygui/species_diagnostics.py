import os

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'species_diagnostics.ui'))


class SpeciesDiagnostics(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)
        self.setup_figure()

    def update(self, species):
        self.load_values(species)
        self.make_plot(species)

    def load_values(self, species):
        num_organisms = len(species.organisms)
        current_best = max(org.fitness for org in species.organisms)
        total_adj_fitness = sum(org.adj_fitness for org in species.organisms)
        self.ui.num_organisms.setText(str(num_organisms))
        self.ui.best_historical_fitness.setText(str(species.best_fitness))
        self.ui.age.setText(str(species.age))
        self.ui.best_fitness.setText(str(current_best))
        self.ui.adj_fitness.setText(str(total_adj_fitness))

    def setup_figure(self):
        self.figure = self.ui.matplotlibCanvas.figure
        self.canvas = self.ui.matplotlibCanvas.canvas

    def make_plot(self, species):
        self.figure.clear()

        axes = self.figure.add_subplot(1,1,1)
        axes.hist([org.fitness for org in species.organisms])
        axes.set_xlabel('Fitness')
        axes.set_ylabel('Count')
        self.figure.tight_layout()

        self.canvas.draw()
