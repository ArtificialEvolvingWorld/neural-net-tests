import os
import sys

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'organism_diagnostics.ui'))

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
        self.figure = self.ui.matplotlibCanvas.figure
        self.canvas = self.ui.matplotlibCanvas.canvas

    def make_plot(self):
        net = self.organism.network
        g = graph.Graph()

        for i,node_type in enumerate(net.node_types):
            color = {pyneat.NodeType.Input: 'green',
                     pyneat.NodeType.Bias: 'gray',
                     pyneat.NodeType.Hidden: 'blue',
                     pyneat.NodeType.Output: 'red',
            }[node_type]

            fixed_y = {pyneat.NodeType.Input: 0,
                       pyneat.NodeType.Bias: 0,
                       pyneat.NodeType.Hidden: None,
                       pyneat.NodeType.Output: 1,
            }[node_type]

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
