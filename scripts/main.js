$( document ).ready(function() {
    console.log('ready to light up!');

    $('.slider').slider({
		value: 201,
		min: 1,
		max: 5000,
		step: 100,
		slide: function( event, ui ) {
			$(this).parent().parent().attr('data-timeout', ui.value);
			$(this).parent().find('span').html(ui.value - 1);
		}
    });

    $('.btn').click(function(e) {
		e.preventDefault();
		var adres = $(this).parent().attr('data-ip');
		var red = $(this).attr('data-red');
		var green = $(this).attr('data-green');
		var blue = $(this).attr('data-blue');
		var timeout = $(this).parent().attr('data-timeout');
		var arduinoURL = 'http://' + adres + '/arduino/led/';

		$.ajax({
			url: arduinoURL + timeout + '/' + red + '/' + green + '/' + blue + '/',
			dataType: 'jsonp',
			jsonp: 'jsonp',
			beforeSend: function( xhr ) {
				console.log(xhr);
			},
			success: function (e) {
				var response = jQuery.parseJSON(e);
				console.log(response.status);
			}
		});
    });
});