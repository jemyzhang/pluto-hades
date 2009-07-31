from PyQt4 import QtCore, QtGui


from PyQt4.QtCore import *
from PyQt4.QtGui import *



class VisualFeedback(QtGui.QGraphicsItem):
    def __init__(self, parent):
        super(VisualFeedback, self).__init__()
        
        self.timeline = QTimeLine()
        self.timeline.setDuration(2000)
        self.timeline.setFrameRange(0, 11)
        self.timeline.setCurveShape(QTimeLine.SineCurve)
        self.timeline.frameChanged.connect(self.animate)
        self.timeline.finished.connect(self.end)
        
        self.frame = 0
        
        self.color = QColor("blue")
        
        
    def start(self):
        self.timeline.stop()
        
        self.frame = 0
        self.timeline.setCurrentTime(0)
        self.timeline.start()
        
        self.show()
        self.update()
        
    def end(self):
        self.hide()
        
        
    def animate(self, frame):
        self.frame = frame % 11
        self.update()
        
        print (self.frame, frame)
        
    def setColor(self, color):
        self.color = color
        
    def boundingRect(self):
        return QRectF(-50, -50, 100, 100)
        
    def paint(self, painter, option, widget):
        alpha = 180
        penWidth = 4.5
        rect = QRect(-8, -8, 16, 16)
        
        color = self.color
        color.setAlpha(alpha)
        
        pen = QPen()
        pen.setWidth(penWidth)
        pen.setColor(color)        
        
        painter.setRenderHints(QtGui.QPainter.Antialiasing)
        painter.setBrush(color)
        painter.setPen(pen)
        
        for frame in range(1, self.frame):
            painter.drawEllipse(rect)
        
            alpha -= 20
            penWidth -= 0.3
            rect.adjust(-5, -5, 5, 5)
            
            color.setAlpha(alpha)
            
            pen.setColor(color)
            pen.setWidthF(penWidth)
            
            if (frame > 3):
                pen.setStyle(QtCore.Qt.DashLine)
            
            painter.setPen(pen)
            painter.setBrush(QtCore.Qt.NoBrush)
            
            #print (alpha, penWidth, rect)
    
    
class View(QGraphicsView):
    def __init__(self, parent):
        super(View, self).__init__()
    
        
        gscene = QtGui.QGraphicsScene()
        self.setScene(gscene)
        
        self.vf = VisualFeedback(gscene)
        gscene.addItem(self.vf)
        self.vf.setPos(0, 0)
        
        self.vf2 = VisualFeedback(gscene)
        gscene.addItem(self.vf2)
        self.vf2.setPos(0, 100)
        self.vf2.setColor(QColor(0x0197fd))
        
        self.vf3 = VisualFeedback(gscene)
        gscene.addItem(self.vf3)
        self.vf3.setPos(0, 200)
        self.vf3.setColor(QColor(0x015a01))
        
        self.vf.start()
        self.vf2.start()
        self.vf3.start()
        
    def mouseReleaseEvent(self, event):
        self.vf2.setPos(self.mapToScene(event.pos()))
        self.vf2.start()
    
if __name__ == '__main__':

    import sys

    app = QtGui.QApplication(sys.argv)
    
    view = View(app)
    view.show()
    
    image = QImage("I0000001.jpg");
    ba = QByteArray()
    print (image.numBytes())
    
    ds = QDataStream(ba, QIODevice.WriteOnly)
    
    ds<<image
    
    print (ba.size())

    sys.exit(app.exec_())
