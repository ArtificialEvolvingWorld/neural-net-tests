import os
import sys

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'custom_network_seed_tab.ui'))

import pyneat

from pygui.net_parse import parse_net

class CustomNetworkSeedTab(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

        self.setup_figure()
        self.make_plot()

    def setup_figure(self):
        self.figure = self.ui.matplotlibCanvas.figure
        self.canvas = self.ui.matplotlibCanvas.canvas

    def make_plot(self):
        self.figure.clear()
        axes = self.figure.add_subplot(1,1,1)
        axes.text(0, 0, 'Todo:\nLive Drawing of Network',
                  horizontalalignment='center',
                  verticalalignment='center')
        axes.set_xlim(-1,1)
        axes.set_ylim(-1,1)

        self.canvas.update()

    def init(self, config):
        num_inputs = config.num_inputs
        num_outputs = config.num_outputs

        inputs = ['In'+str(i) for i in range(num_inputs)]
        outputs = ['Out'+str(i) for i in range(num_outputs)]

        text = ['Input: ' + ', '.join(inputs),
                'Output: ' + ', '.join(outputs),
                ]

        if config.output_activation_func is not None:
            func_str = str(config.output_activation_func).split('.')[-1]
            text.append(func_str + ': ' + ', '.join(outputs))

        text.extend(
            '{} --> {}'.format(in_node, out_node)
            for in_node in inputs + ['bias']
            for out_node in outputs
            )

        self.ui.textEdit.setText('\n'.join(text))

    def make_seed(self):
        text = self.ui.textEdit.toPlainText()
        seed = parse_net(text)
        return seed
