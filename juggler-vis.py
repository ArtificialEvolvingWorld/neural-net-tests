#!/usr/bin/env python3

import math
import os
import sys

sys.path.append(os.path.join(os.path.dirname(__file__), 'lib'))
import Visualizations
import TestCases

def main():
    viewer = Visualizations.VisualTestViewer(800, 600)

    # Make camera
    cam = viewer.make_camera()
    cam.mode = Visualizations.CameraType.PinToSurface
    cam.position = Visualizations.vector3df(10,-10,10)
    cam.look_at(Visualizations.vector3df(0,0,0))
    cam.movement_speed = 20

    # Make juggler
    initial = TestCases.Juggler(2,3)
    initial.hand_positions[0] = TestCases.GVector2(-1, 0)
    initial.hand_positions[1] = TestCases.GVector2(+1, 0)
    initial.ball_positions[0] = TestCases.GVector2(-1, 1)
    initial.ball_positions[1] = TestCases.GVector2(+1, 1)
    initial.ball_velocities[1] = TestCases.GVector2(0, 5)
    initial.ball_positions[2] = TestCases.GVector2(0, 5)
    initial.hand_size = 2
    initial.close_hand(0)
    entity = viewer.make_juggler(initial)

    # Run
    viewer.run()


if __name__=='__main__':
    main()
