import os

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'species_diagnostics.ui'))


class SpeciesDiagnostics(QWidget):
    def __init__(self, species, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

        self.species = species
        self.load_values()

    def load_values(self):
        num_organisms = len(self.species.organisms)
        self.ui.num_organisms.setText(str(num_organisms))
        self.ui.best_fitness.setText(str(self.species.best_fitness))
        self.ui.age.setText(str(self.species.age))
