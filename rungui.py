#!/usr/bin/env python3

import sys
from PyQt4 import QtGui

from pygui.main_window import MainWindow

def main():
    app = QtGui.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())

if __name__=='__main__':
    main()
