import os

from PyQt4 import uic, QtCore, QtGui

(Ui_Widget, QWidget) = uic.loadUiType(
    os.path.join(os.path.dirname(__file__),'xor_diagnostics.ui'))

class XorDiagnostics(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_Widget()
        self.ui.setupUi(self)

    def update(self, org, opts):
        widgets = [(0,0,self.ui.xor_00),
                   (0,1,self.ui.xor_01),
                   (1,0,self.ui.xor_10),
                   (1,1,self.ui.xor_11),
                   ]
        if org is None:
            for _,_,wid in widgets:
                wid.setText('')

        else:
            for a,b,wid in widgets:
                res = org.network.evaluate([a,b])[0]
                wid.setText('{:.3f}'.format(res))
