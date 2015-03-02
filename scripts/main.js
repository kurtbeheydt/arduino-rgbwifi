$( document ).ready(function() {
    console.log('ready to light up!');
    var lockState = false;
    var locksliders = $('#locksliders');

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

    locksliders.click(function (e) {
    	e.preventDefault();
    	var secondary = $('.secondary');

    	if (lockState) {
    		secondary.show();
			locksliders.html('start simultane modus');
    		lockState = false;
    	} else {
    		secondary.hide();
			locksliders.html('beëindig simultane modus');
    		lockState = true;
    	}
    });

    $('.btn').click(function(e) {
		e.preventDefault();

		var red = $(this).attr('data-red');
		var green = $(this).attr('data-green');
		var blue = $(this).attr('data-blue');
		var effect = $(this).parent().find('.effect:checked').val();
		var timeout = $(this).parent().attr('data-timeout');

		var arduinoURLs = [];
		
		if (lockState) {
			$('.unit').each(function() {
				var adres = $(this).attr('data-ip');
				arduinoURLs.push('http://' + adres + '/arduino/led/');
			});
		} else {
			var adres = $(this).parent().attr('data-ip');
			arduinoURLs.push('http://' + adres + '/arduino/led/');
		}

		for (var j = 0;j<=1;j++) {
			$.each( arduinoURLs, function( key, arduinoURL ) {
				$.ajax({
					url: arduinoURL + timeout + '/' + red + '/' + green + '/' + blue + '/' + effect + "/",
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
		}	
    });
});