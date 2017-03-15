import os
import sys

from PyQt4 import uic, QtCore, QtGui

import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QT as NavigationToolbar

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'organism_diagnostics.ui'))

from pygui.util import fill_placeholder
import pyneat

# TODO: Cleaner way to import from graph_display
sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, "graph_display"))
import graph

class OrganismDiagnostics(QWidget):
    def __init__(self, organism, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

        self.organism = organism
        self.load_values()
        self.setup_figure()
        self.make_plot()

    def load_values(self):
        self.ui.num_nodes.setText(str(self.organism.network.num_nodes))
        self.ui.num_connections.setText(str(self.organism.network.num_nodes))
        self.ui.fitness.setText(str(self.organism.fitness))
        self.ui.adj_fitness.setText(str(self.organism.adj_fitness))

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
        net = self.organism.network
        g = graph.Graph()

        for i,node_type in enumerate(net.node_types):
            color = {pyneat.NodeType.Input:        '#2124FF',
                     pyneat.NodeType.Bias:         '#1F22C1',
                     pyneat.NodeType.Output:       '#FF2718',
                     pyneat.NodeType.Sigmoid:      '#EE8A2A',
                     pyneat.NodeType.Tanh:         '#B17516',
                     pyneat.NodeType.Relu:         '#B1B0AA',
                     pyneat.NodeType.Gaussian:     '#2CB11F',
                     pyneat.NodeType.Sin:          '#F6DE39',
                     pyneat.NodeType.Cos:          '#C5B12C',
                     pyneat.NodeType.Abs:          '#E685E7',
                     pyneat.NodeType.Mult:         '#257580',
                     pyneat.NodeType.Add:          '#68C6D3',
                     pyneat.NodeType.MultGaussian: '#3E8727',
                     pyneat.NodeType.Square:       '#F050E6'
                 }[node_type]
            try:
                fixed_y = {pyneat.NodeType.Input: 0,
                           pyneat.NodeType.Bias: 0,
                           pyneat.NodeType.Output: 1,
                       }[node_type]
            except:
                fixed_y = None

            g.add_node(i, color=color)
            if fixed_y is not None:
                g.fix_y(i, fixed_y)


        for conn in net.connections:
            boxed = (conn.type == pyneat.ConnectionType.Recurrent)
            g.add_connection(conn.origin, conn.dest, boxed=boxed)

        self.figure.clear()
        axes = self.figure.add_subplot(1,1,1)
        g.draw(axes)

        self.canvas.update()
