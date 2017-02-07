from PyQt4 import QtGui

def fill_placeholder(placeholder, widget):
    if placeholder.layout() is not None:
        QtGui.QWidget().setLayout(placeholder.layout())
    layout = QtGui.QVBoxLayout(placeholder)
    layout.addWidget(widget)
    placeholder.setLayout(layout)
