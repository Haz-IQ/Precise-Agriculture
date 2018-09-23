/**
 * Created by Gigabyte-Pc on 9/22/2018.
 */
var endpoint = 'api/chart/data/'
var defaultdata = []
labels = []
$.ajax({
	method: "GET",
	url : endpoint,
	success: function(data){
		labels = data.labels
		defaultdata = data.defaultdata
		var ctx = document.getElementById("myChart");
		var myChart = new Chart(ctx, {
			type: 'line',
			data: {
				labels: labels,
				datasets: [{
					label: '# of adsdsa',
					data: defaultdata,
					backgroundColor: [
						'rgba(255, 99, 132, 0.2)',
						'rgba(54, 162, 235, 0.2)',
						'rgba(255, 206, 86, 0.2)',
						'rgba(0, 192, 192, 0.2)',
						'rgba(153, 102, 255, 0.2)',
						'rgba(255, 159, 64, 0.2)'
						],
					borderColor: [
						'rgba(255,255,255,1)',
						'rgba(54, 162, 235, 1)',
						'rgba(255, 206, 86, 1)',
						'rgba(75, 192, 192, 1)',
						'rgba(153, 102, 255, 1)',
						'rgba(255, 159, 64, 1)'
					],
					borderWidth: 1
				}]
			},
			options: {
				responsive: false,
				maintainAspectRatio: true,
				scales: {
					yAxes: [{
						ticks: {
							beginAtZero:true
						}
					}]
				}
			}
		});
	},
	error : function(error_data){
		console.log("error")
		console.log(error_data)
	}
})
