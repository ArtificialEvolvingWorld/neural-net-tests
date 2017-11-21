#!/bin/bash

cd $(dirname "${BASH_SOURCE[0]}")

mkdir -p deps
cd deps

wget -O plotly-latest.min.js https://cdn.plot.ly/plotly-latest.min.js
wget -O vis.min.js https://cdnjs.cloudflare.com/ajax/libs/vis/4.21.0/vis.min.js
wget -O vis.min.css https://cdnjs.cloudflare.com/ajax/libs/vis/4.21.0/vis.min.css
