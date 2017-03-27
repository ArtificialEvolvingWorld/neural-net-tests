import math
import os

import numpy as np

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'single_pendulum_diagnostics.ui'))

import TestCases

def normalize_theta(theta):
    n = round(theta / (2*math.pi))
    return theta - n*(2*math.pi)

class SinglePendulumDiagnostics(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

    def update(self, org, opts):
        figure = self.ui.matplotlibCanvas.figure
        figure.clear()

        if org is None:
            return


        rk = TestCases.SinglePendulum_RungeKutta(
            opts['initial_state'], opts['deltaT'])

        all_times = []
        all_theta = []
        #for step_num in range(opts['num_steps']):
        for step_num in range(opts['num_steps']//100):
            this_theta = normalize_theta(rk.current.theta)
            all_theta.append(this_theta)
            all_times.append(rk.time)


            val = org.network.evaluate([this_theta])[0]
            rk.current.Fext = 5*(2*val-1)

            rk.Step()


        axes = self.ui.matplotlibCanvas.figure.add_subplot(1,1,1)
        axes.plot(all_times, np.degrees(all_theta), color='black')
        limit = np.degrees(opts['max_angle_balanced'])
        axes.plot([all_times[0], all_times[-1], None, all_times[0], all_times[-1]],
                  [limit, limit, None, -limit, -limit],
                  color='red')
        axes.set_xlabel('Time')
        axes.set_ylabel('Theta')

        figure.tight_layout()
