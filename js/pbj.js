(function($) {

Drupal.behaviors.pbj = {
	attach: function(context, settings) {
		if (!Drupal.settings.pbj) return;
		
		Drupal.behaviors.pbj.editorSettings = Drupal.settings.pbj.settings;

		/**
		* Bind the change event to all text format select lists.
		*/
		$('div.text-format-wrapper fieldset.filter-wrapper select.filter-list').once('pbj').bind('change', function(e) {
			console.log('hit');
			var $textFormatWrapper = $(this).parents('div.text-format-wrapper:first');
			Drupal.behaviors.pbj.pbjify($textFormatWrapper);
		});

		$('#pbj-upload').once('pbj').click(function(){
			$('#pbj-image-upload .form-file').trigger('click');
			return false;
		});

		$('#pbj-image-upload .form-file').once('pbj').change(function(){
			setTimeout(function(){
				$('#pbj-submit').hide().triggerHandler('mousedown');
			}, 100);
		});

		$('#pbj .pbj-image-size a').once('pbj').click(function(){
			$('#pbj .pbj-image-size a.active').removeClass('active');
			var value = $(this).attr('data-value');
			$('#pbj-size').val(value);
			$(this).addClass('active');
			return false;
		}).each(function(){
			if($(this).hasClass('active')){
				var value = $(this).attr('data-value');
				$('#pbj-size').val(value);
			}
		});

		$('#pbj .pbj-image-position a').once('pbj').click(function(){
			$('#pbj .pbj-image-position a.active').removeClass('active');
			var value = $(this).attr('data-value');
			$('#pbj-position').val(value);
			$(this).addClass('active');
			return false;
		}).each(function(){
			if($(this).hasClass('active')){
				var value = $(this).attr('data-value');
				$('#pbj-position').val(value);
			}
		});
		
	},

	editorSettings: {},

	pbjify: function($textFormatWrapper){
		// The select list for chosing the text format that will be used.
		var $filterSelector = $textFormatWrapper.find('select.filter-list');
		var settings = Drupal.behaviors.pbj.editorSettings;
			
		// Checks if the currently selected filter is one that uses the editor.
		if ($.inArray($filterSelector.val(), settings.text_formats) != -1) {
			$('.wmd-preview').show();
			$('.wmd-panel .form-textarea-wrapper').addClass('wmd-input-active');
			var converter1 = Markdown.getSanitizingConverter();
   		var editor1 = new Markdown.Editor(converter1);
   		editor1.run();
		} else { // Show the textarea.
			console.log('nope');
			$('.wmd-preview').hide();
			$('.wmd-button-bar').html('');
			$('.wmd-input-active textarea').css('height','');
			$('.wmd-panel .form-textarea-wrapper').removeClass('wmd-input-active');
		}

	}
};

})(jQuery);