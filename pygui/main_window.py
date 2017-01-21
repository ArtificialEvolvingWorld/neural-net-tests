import os
from PyQt4 import uic, QtGui, QtCore

(Ui_MainWindow, QMainWindow) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'main_window.ui'))

class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        self.ui.advance_one_gen.clicked.connect(self.advance_one_gen)
        self.ui.advance_ten_gen.clicked.connect(self.advance_ten_gen)
        self.ui.advance_n_gen.clicked.connect(self.advance_n_gen)
        self.ui.mutation_rate.valueChanged.connect(self.mutation_rate_changed)

    def advance_one_gen(self):
        print('Advance one generation')

    def advance_ten_gen(self):
        print('Advance ten generation')

    def advance_n_gen(self):
        n = self.ui.num_gens.value()
        print('Advance {} generation'.format(n))

    def mutation_rate_changed(self,value):
        print('Mutation rate changed to {}'.format(value))
