import os

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'population_diagnostics.ui'))

class PopulationDiagnostics(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)
        self.setup_figure()

    def update(self, population):
        self.load_values(population)
        self.make_plot(population)

    def load_values(self, population):
        num_species = len(population.species)
        num_organisms = sum(len(spec.organisms) for spec in population.species)
        best_fitness = max(org.fitness
                           for spec in population.species
                           for org in spec.organisms)
        total_adj_fitness = sum(org.adj_fitness
                                for spec in population.species
                                for org in spec.organisms)

        self.ui.num_organisms.setText(str(num_organisms))
        self.ui.num_species.setText(str(num_species))
        self.ui.best_fitness.setText(str(best_fitness))
        self.ui.total_adj_fitness.setText(str(total_adj_fitness))

    def setup_figure(self):
        self.figure = self.ui.matplotlibCanvas.figure
        self.canvas = self.ui.matplotlibCanvas.canvas

    def make_plot(self, population):
        self.figure.clear()

        axes = self.figure.add_subplot(1,1,1)
        axes.hist([org.fitness
                   for spec in population.species
                   for org in spec.organisms])
        axes.set_xlabel('Fitness')
        axes.set_ylabel('Count')
        self.figure.tight_layout()

        self.canvas.draw()
