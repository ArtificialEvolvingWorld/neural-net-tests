from PyQt4 import uic, QtCore, QtGui

import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar

class MatplotlibCanvas(QtGui.QWidget):
    def __init__(self, parent=None):
        super(MatplotlibCanvas, self).__init__(parent)
        bg = self.palette().window().color()
        color = (bg.redF(), bg.greenF(), bg.blueF())
        
        self.figure = plt.figure(edgecolor=color, facecolor=color)
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas, self)
        self.toolbar.pan()
        self.toolbar.hide()

        layout = QtGui.QVBoxLayout()
        layout.addWidget(self.canvas)
        layout.addWidget(self.toolbar)
        self.setLayout(layout)
        

        
    

