// var x = [1,1,1,2,2,2,NaN,4,6,5,5,6,4];
// var y = [1,3,2,2,3,1,NaN,1,1,1,2,2,2];

// var plot_data = { x: x,
//                   y: y,
//                   mode: 'lines',
//                   type: 'scatter',
//                   name: 'Some plot'
//                 };

var rand = []
for(var i=0; i<1000; i+=1) {
    rand[i] = Math.random();
}

var plot_data = { x: rand,
                  type: 'histogram',
                  name: 'Some plot'
                };

var layout = { margin: { t: 40 },
               title: 'Dummy Plot',
               yaxis: {
                   title: 'Y-axis title',
                   //range: [display_min, display_max]
               },
               showlegend: false };

var options = { showLink: false,
                staticPlot: false,
                displayModeBar: false };

Plotly.plot('population-stat-box', [plot_data], layout, options);
