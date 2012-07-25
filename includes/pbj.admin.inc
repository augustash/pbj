<?php

/**
 * Implements hook_form().
 *
 * Settings form to configure the module.
 */
function pbj_settings_form($form, &$form_state) {
	$form = array();

	$filter_formats = filter_formats();
	$filter_formats_options = array();
	foreach($filter_formats as $key => $format_obj) {
		$filter_formats_options[$key] = $format_obj->name;
	}
	
	$form['explanation'] = array(
		'#markup' => t('These settings affect the editor only when used to edit HTML/PHP in your nodes or blocks.<br><br>'),
	);

	$form['pbj_filter_formats'] = array(
		'#type' => 'select',
		'#title' => t('Text formats'),
		'#description' => t('Select the text formats that will be using the editor.'),
		'#multiple' => TRUE,
		'#options' => $filter_formats_options,
		'#default_value' => variable_get('pbj_filter_formats', array()),
		'#size' => count($filter_formats_options),
		'#attributes' => array(
			'style' => 'min-width: 400px; margin-top: 10px;'
		),
	);

	$default = ".wmd-preview img.float-left{float:left;margin: 5px 20px 20px 0;}\n.wmd-preview img.float-right{float:right;margin: 5px 0 20px 20px;}\n.wmd-preview img.center{margin:0 auto;}\n.wmd-preview img{display:block;}\n.wmd-preview ol li{list-style-type: decimal;}\n.wmd-preview ul li{list-style-type: disc;}";
	$form['pbj_styles'] = array(
		'#type' => 'textarea',
		'#title' => t('CSS Styles to use in preview area'),
		'#description' => t('The preview area is contained in a div with the class of .wmd-preview'),
		'#default_value' => variable_get('pbj_styles', $default),
	);

	$form['pbj_width'] = array(
		'#type' => 'textfield',
		'#title' => t('Width of preview area'),
		'#default_value' => variable_get('pbj_width', ''),
	);

  // Provide a select field for choosing an image style to use when displaying
  // the image.
  $form['pbj_imagestyles'] = array(
  	'#type' => 'fieldset',
  	'#title' => 'Image styles'
  );
  $form['pbj_imagestyles']['pbj_imagestyle_small'] = array(
    '#title' => t('Small'),
    '#type' => 'select',
    '#options' => image_style_options(TRUE),
    '#default_value' => variable_get('pbj_imagestyle_small', 'thumbnail'),
  );
  $form['pbj_imagestyles']['pbj_imagestyle_medium'] = array(
    '#title' => t('Medium'),
    '#type' => 'select',
    '#options' => image_style_options(TRUE),
    '#default_value' => variable_get('pbj_imagestyle_medium', 'medium'),
  );
  $form['pbj_imagestyles']['pbj_imagestyle_large'] = array(
    '#title' => t('Large'),
    '#type' => 'select',
    '#options' => image_style_options(TRUE),
    '#default_value' => variable_get('pbj_imagestyle_large', 'large'),
  );

	return system_settings_form($form);
}