import os

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'organism_diagnostics.ui'))


class OrganismDiagnostics(QWidget):
    def __init__(self, organism, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

        self.organism = organism
        self.load_values()

    def load_values(self):
        self.ui.num_nodes.setText(str(self.organism.network.num_nodes))
        self.ui.num_connections.setText(str(self.organism.network.num_nodes))
        self.ui.fitness.setText(str(self.organism.fitness))
        self.ui.adj_fitness.setText(str(self.organism.adj_fitness))
