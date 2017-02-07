import os

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'population_diagnostics.ui'))


class PopulationDiagnostics(QWidget):
    def __init__(self, population, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

        self.population = population
        self.load_values()

    def load_values(self):
        num_species = len(self.population.species)
        num_organisms = sum(len(spec.organisms) for spec in self.population.species)
        best_fitness = max(org.fitness
                           for spec in self.population.species
                           for org in spec.organisms)

        self.ui.num_organisms.setText(str(num_organisms))
        self.ui.num_species.setText(str(num_species))
        self.ui.best_fitness.setText(str(best_fitness))