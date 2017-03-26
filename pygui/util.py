from PyQt4 import QtGui

def fill_placeholder(placeholder, widget):
    if placeholder.layout() is None:
        placeholder.setLayout(QtGui.QVBoxLayout(placeholder))

    layout = placeholder.layout()

    while layout.count():
        widget_to_delete = layout.itemAt(0).widget()
        layout.removeWidget(widget_to_delete)
        widget_to_delete.close()

    if widget is not None:
        layout.addWidget(widget)
