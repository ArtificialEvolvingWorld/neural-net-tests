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
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)
        self.setup_figure()
        self.graph = None

    def update(self, organism):
        self.load_values(organism)
        self.make_plot(organism)

    def load_values(self, organism):
        self.ui.num_nodes.setText(str(organism.network.num_nodes))
        self.ui.num_connections.setText(str(organism.network.num_nodes))
        self.ui.fitness.setText(str(organism.fitness))
        self.ui.adj_fitness.setText(str(organism.adj_fitness))

    def setup_figure(self):
        self.figure = self.ui.matplotlibCanvas.figure
        self.canvas = self.ui.matplotlibCanvas.canvas

    def stop_graph(self):
        if self.graph is not None:
            self.graph.stop()
            self.graph = None

    def make_plot(self, organism):
        self.stop_graph()

        net = organism.network
        self.graph = graph.Graph()

        for i,(node_type,func) in enumerate(zip(net.node_types, net.activation_functions)):
            color_table_type = {
                pyneat.NodeType.Input:                  '#2124FF',
                pyneat.NodeType.Bias:                   '#1F22C1',
                pyneat.NodeType.Output:                 '#FF2718',
            }

            color_table_func = {
                pyneat.ActivationFunction.Identity:     '#68C6D3',
                pyneat.ActivationFunction.Sigmoid:      '#EE8A2A',
                pyneat.ActivationFunction.Tanh:         '#B17516',
                pyneat.ActivationFunction.Relu:         '#B1B0AA',
                pyneat.ActivationFunction.Gaussian:     '#2CB11F',
                pyneat.ActivationFunction.Sin:          '#F6DE39',
                pyneat.ActivationFunction.Cos:          '#C5B12C',
                pyneat.ActivationFunction.Abs:          '#E685E7',
                pyneat.ActivationFunction.Square:       '#F050E6',
            }

            if node_type in color_table_type:
                color = color_table_type[node_type]
            else:
                color = color_table_func[func]

            try:
                fixed_y = {pyneat.NodeType.Input: 0,
                           pyneat.NodeType.Bias: 0,
                           pyneat.NodeType.Output: 1,
                       }[node_type]
            except:
                fixed_y = None

            self.graph.add_node(i, color=color)
            if fixed_y is not None:
                self.graph.fix_y(i, fixed_y)


        for conn in net.connections:
            boxed = (conn.type == pyneat.ConnectionType.Recurrent)
            self.graph.add_connection(conn.origin, conn.dest, boxed=boxed)

        self.figure.clear()
        axes = self.figure.add_subplot(1,1,1)
        self.graph.draw(axes)

        self.canvas.update()
