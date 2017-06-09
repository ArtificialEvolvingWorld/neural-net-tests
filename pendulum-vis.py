#!/usr/bin/env python3

import math
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), 'lib'))
import Visualizations
import TestCases

#include <memory>

#include "VisualTestViewer.hh"
#include "DoublePendulum.hh"
#include "DoublePendulumEntity.hh"
#include "TwoPendulum.hh"
#include "TwoPendulumEntity.hh"
#include "SinglePendulum.hh"
#include "SinglePendulumEntity.hh"
#include "CameraEntity.hh"

def main():
    viewer = Visualizations.VisualTestViewer(800, 600)

    cam = viewer.make_camera()
    cam.mode = Visualizations.CameraType.PinToSurface
    cam.position = Visualizations.vector3df(10,-10,10)
    cam.look_at(Visualizations.vector3df(0,40,0))
    cam.movement_speed = 20

    for i in range(101):
        initial = TestCases.DoublePendulum()
        initial.length2 = 2
        initial.mass2 = 1
        initial.length1 = 4
        initial.mass1 = 2
        initial.theta1 = math.radians((100-i)/100.0)

        entity = viewer.make_double_pendulum(initial)
        entity.position = Visualizations.vector3df(-10, i*3, 0)

    for i in range(101):
        initial = TestCases.TwoPendulum()
        initial.length2 = 2
        initial.mass2 = 1
        initial.length1 = 4
        initial.mass1 = 2
        initial.theta1 = math.radians((100-i)/100.0)

        entity = viewer.make_two_pendulum(initial)
        entity.position = Visualizations.vector3df(+10, i*3, 0)

    for i in range(101):
        initial = TestCases.SinglePendulum()
        initial.length = 2
        initial.theta = math.radians((100-i)/100.0)

        entity = viewer.make_single_pendulum(initial)
        entity.position = Visualizations.vector3df(+30, i*3, 0);

    viewer.run()


if __name__=='__main__':
    main()
