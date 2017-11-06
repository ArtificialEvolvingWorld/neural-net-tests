#!/bin/bash

cd $(dirname "${BASH_SOURCE[0]}")

mkdir -p deps
cd deps

wget https://cdn.plot.ly/plotly-latest.min.js
