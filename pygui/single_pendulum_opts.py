import math
import os

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'single_pendulum_opts.ui'))

import TestCases

class SinglePendulumOptions(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

    def options(self):
        initial = TestCases.SinglePendulum()
        initial.mass_cart = self.ui.mass_cart.value()
        initial.mass_rod = self.ui.mass_rod.value()
        initial.length = self.ui.rod_length.value()
        initial.theta = math.radians(self.ui.initial_angle.value())

        max_angle_balanced = math.radians(self.ui.max_angle_balanced.value())

        return {'initial_state': initial,
                'deltaT': self.ui.delta_t.value(),
                'num_steps': self.ui.num_time_steps.value(),
                'max_angle_balanced': max_angle_balanced,
            }
