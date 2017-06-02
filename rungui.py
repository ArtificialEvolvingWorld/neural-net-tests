#!/usr/bin/env python3

import os
import sys
import argparse

from PyQt4 import QtGui

sys.path.append(os.path.join(os.path.dirname(__file__),'lib'))

from pygui.main_window import MainWindow

def get_program_arguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("--seed", type=int,help="Pseudo-random number generator seed to be used. [Default: Time since epoch]",default=None)
    args = parser.parse_args()
    return args;


def main():
    args = get_program_arguments()
    app = QtGui.QApplication(sys.argv)
    window = MainWindow(args)
    window.show()
    sys.exit(app.exec_())

if __name__=='__main__':
    main()
