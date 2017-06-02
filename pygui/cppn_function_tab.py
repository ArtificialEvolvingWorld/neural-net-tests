from PyQt4 import QtCore, QtGui

class CPPNFunctionTab(QtGui.QWidget):
    def __init__(self, prob, parent=None):
        super().__init__(parent)

        self.prob = prob
        self.spinboxes = {}

        self.setLayout(QtGui.QGridLayout(self))
        self.generate_labels()

    def generate_labels(self):
        layout = self.layout()


        for i,(func,odds) in enumerate(self.prob.cppn_odds.items()):
            label = QtGui.QLabel(self)
            label.setText(str(func))
            layout.addWidget(label, i, 0)

            spinbox = QtGui.QSpinBox(self)
            spinbox.setMinimum(0)
            spinbox.setMaximum(100)
            spinbox.setValue(odds)
            spinbox.valueChanged.connect(self.load_values_to_cpp)
            layout.addWidget(spinbox, i, 1)

            self.spinboxes[type(func)(func)] = spinbox


    def load_values_from_cpp(self):
        for func,spinbox in self.spinboxes.items():
            value = self.prob.cppn_odds[func]
            spinbox.blockSignals(True)
            spinbox.setValue(value)
            spinbox.blockSignals(False)


    def load_values_to_cpp(self):
        for func,spinbox in self.spinboxes.items():
            value = spinbox.value()
            self.prob.cppn_odds[func] = value
