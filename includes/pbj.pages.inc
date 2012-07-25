<?php

/**
 * Router to display various image browser pages
 *
 * @param $id
 *   The form build id of parent form
 * @param $op
 *   The operation requested. Defaults to browse.
 * @param $a1
 *   An optional variable that ofted contains the FID
 */
function _pbj_image_dashboard($id, $op = 'browse', $a1 = NULL){
  $output = '';
  $commands = array();
  $node = _pbj_node_from_form_id($id);
  _pbj_load('image', $node);
  switch($op){
    case 'image':
      if($a1 && node_access('update', $node)){
        $output = _pbj_image_select($node, $a1);
        $commands[] = ajax_command_css('.modal-footer', array('display'=>'block'));
      $commands[] = ajax_command_css('.modal-footer .btn-primary', array('display'=>''));
      }
      break;
    case 'delete':
      if($a1 && node_access('update', $node)){
        $output = render(drupal_get_form('_pbj_image_delete_form', $node, $a1));
        $commands[] = ajax_command_css('.modal-footer', array('display'=>'none'));
      }
      break;
    default:
      $output = render(drupal_get_form('_pbj_image_dashboard_form', $node));
      $commands[] = ajax_command_css('.modal-footer', array('display'=>''));
      $commands[] = ajax_command_css('.modal-footer .btn-primary', array('display'=>'none'));
      break;

  }
  $commands[] = ajax_command_html('.modal-body', '<div id="pbj">'.$output.'</div>');
  $commands[] = ajax_command_prepend('.modal-body', theme('status_messages'));     
  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Image delete form
 *
 * @param $node
 *   The node object
 */
function _pbj_image_dashboard_form($form, &$form_state, $node){

  // Make sure to include this file so validation function fires
  form_load_include($form_state, 'inc', 'pbj', 'includes/pbj.pages');

  $form_state['node'] = $node;
  $field_name = variable_get('pbj_field_' . $node->type, FALSE);
  
  // If we don't have a setting for this content type lets just return
  if(!$field_name) return array('none'=>array('#markup'=>t('Image browser not enabled for this content type.')));

  $entity = 'node';
  $bundle = $node->type;
  $field = field_info_field($field_name);
  $ct_instances = field_info_instances($entity, $bundle);
  $instance = $ct_instances[$field_name];
  // Filefield paths support
  $filepath = $instance['settings']['filefield_paths']['file_path']['value'];

  $form['#id'] = 'pbj-imagebrowser';

  $form['browser'] = array(
    '#markup' => _pbj_image_browser($node, $field_name),
  );

  // Use the #managed_file FAPI element to upload an image file.
  $form['image'] = array(
    '#type' => 'managed_file',
    '#upload_location' => 'public://'.$filepath.'/',
    '#id' => 'pbj-image-upload',
    '#attributes' => array('class'=>array('js-hide')),
  );
  $form['filler'] = array(
    '#markup' => '<a id="pbj-upload" class="btn btn-info pbj-left"><i class="icon-picture icon-white"></i> Upload new image</a>'
  );
  // Submit Button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#id' => 'pbj-submit',
    //'#no_button' => TRUE,
    '#attributes' => array('class'=>array('js-hide')),
    '#ajax' => array(
        'callback' => '_pbj_image_dashboard_form_callback',
        'wrapper' => 'pbj-imagebrowser',
        'effect' => 'fade',
      ),
  );

  return $form;
}

/**
 * Verifies that the user supplied an image with the form..
 */
function _pbj_image_dashboard_form_validate($form, &$form_state) {
  if (!isset($form_state['values']['image']) || !is_numeric($form_state['values']['image']) || (isset($form_state['values']['image']) && $form_state['values']['image'] == 0)) {
    form_set_error('image', t('Please select an image to upload.'));
  }
}

/**
 * Form Builder; Ajax callback
 */
function _pbj_image_dashboard_form_callback($form, &$form_state) {
  // Reload form
  $form['image']['#value'] = '';
  unset($form['image']['filename']);
  return $form;
}

/**
 * Form Builder; Submit function to save new image to parent node
 */
function _pbj_image_dashboard_form_submit($form, &$form_state) {
  // When using the #managed_file form element the file is automatically
  // uploaded an saved to the {file} table. The value of the corresponding
  // form element is set to the {file}.fid of the new file.
  $form['#submit'] = array();

  $node = $form_state['node'];

  // If fid is not 0 we have a valid file.
  if ($form_state['values']['image'] != 0) {
    // The new file's status is set to 0 or temporary and in order to ensure
    // that the file is not removed after 6 hours we need to change it's status
    // to 1. Save the ID of the uploaded image for later use.
    $file = file_load($form_state['values']['image']);
    //$file->status = FILE_STATUS_PERMANENT;
    file_save($file);

    $field_name = variable_get('pbj_field_' . $node->type, FALSE);
    $node->{$field_name}[LANGUAGE_NONE][] = (array) $file;

    // Filefield path integration
    if(module_exists('filefield_paths')){
      // We may not have a nid or title
      $nid = empty($node->nid) ? time() : FALSE;
      if($nid){
        $node->nid = $nid;
        $node->title = $file->timestamp;
      }
      filefield_paths_entity_update($node, 'node');
      if($nid) $node->nid = null;
    }
    _pbj_save('image', $node);
  }
  
  // Flush form
  $form_state['input'] = array();
  $form_state['rebuild'] = TRUE;
}

/**
 * Image browser which shows all available images
 *
 * @TODO use theme function to generate output
 */
function _pbj_image_browser($node, $field_name){  
  $output = array();
  if(empty($node->{$field_name}[$node->language])){
    $output[] = '<div class="well"><div class="alert alert-info" style="margin:0">No images have been added.</div></div>';
  }else{
    $images = array();
    $files = array_reverse($node->{$field_name}[$node->language]);
    foreach($files as $delta => $file){
      $fids[] = $file['fid'];
      $images[] = '<div class="pbj-browser-image">';
      $images[] = '<a href="/pbj/dashboard/'.$node->form_build_id.'/image/'.$file['fid'].'" class="use-ajax pbj-browser-image-link">';
      $images[] = theme('image_style', array('style_name'=>'pbj_thumbnail', 'path' => $file['uri']));
      $images[] = '</a>';
      $images[] = '<a href="/pbj/dashboard/'.$node->form_build_id.'/delete/'.$file['fid'].'" class="pbj-image-delete btn btn-mini btn-danger use-ajax">';
      $images[] = '<i class="icon-remove"></i>';
      $images[] = '</a>';
      $images[] = '</div>';
    }

    $output[] = '<div id="pbj-browser" class="well clearfix">';
    $output[] = '<div id="pbj-browser-inner" style="width:'.((count($files) * 120) + 10).'px">';
    $output = array_merge($output, $images);
    $output[] = '</div>';
    $output[] = '</div>';
  }
  return implode('', $output);
}

/**
 * Image options page
 *
 * @param $node
 *   The node object
 * @param $fid
 *   The fid of the image to be deleted
 */
function _pbj_image_select($node, $fid){

  $image = file_load($fid);

  $output = array();

  $output[] = '<div class="pbj-image-preview">';
  $output[] = theme('image_style', array('style_name'=>'pbj_preview', 'path' => $image->uri));
  $output[] = '</div>';

  $output[] = '<p class="lead">';
  $output[] = t('Select your image options:');
  $output[] = '</p>';
  
  $output[] = '<div class="pbj-image-size"><div class="btn-group">';
  $output[] = '<a href="#" class="btn disabled btn-inverse"><i class="icon-picture"></i> Size</a>';
  $output[] = '<a href="#" class="btn" data-value="'.variable_get('pbj_imagestyle_small', 'thumbnail').'">Small</a>';
  $output[] = '<a href="#" class="btn active" data-value="'.variable_get('pbj_imagestyle_medium', 'medium').'">Medium</a>';
  $output[] = '<a href="#" class="btn" data-value="'.variable_get('pbj_imagestyle_large', 'large').'">Large</a>';
  $output[] = '</div></div>';
  
  $output[] = '<div class="pbj-image-position"><div class="btn-group">';
  $output[] = '<a href="#" class="btn disabled btn-inverse"><i class="icon-align-justify"></i> Position</a>';
  $output[] = '<a href="#" class="btn" data-value="inline-image float-left">Left</a>';
  $output[] = '<a href="#" class="btn active" data-value="inline-image center">Center</a>';
  $output[] = '<a href="#" class="btn" data-value="inline-image float-right">Right</a>';
  $output[] = '</div></div>';

  $textfield = array(
    '#type' => 'textfield',
    '#id' => 'pbj-size',
    '#attributes' => array('class'=>array('js-hide')),
  );
  $output[] = render($textfield);

  $textfield = array(
    '#type' => 'textfield',
    '#id' => 'pbj-position',
    '#attributes' => array('class'=>array('js-hide')),
  );
  $output[] = render($textfield);

  $path = image_style_url('--PBJ--', $image->uri);
  $path = str_replace($GLOBALS['base_url'], '', $path);
  $textfield = array(
    '#type' => 'textfield',
    '#id' => 'pbj-path',
    '#value' => $path,
    '#attributes' => array('class'=>array('js-hide')),
  );
  $output[] = render($textfield);
  
  $output[] = '<div class="pbj-left">'.l('<i class="icon-arrow-left"></i> select a different image', 'pbj/dashboard/'.$node->form_build_id, array('html'=>TRUE, 'attributes'=>array('class'=>array('use-ajax','btn',)))).'</div>';
  return implode('', $output);
}

/**
 * Image delete form
 *
 * @param $node
 *   The node object
 * @param $fid
 *   The fid of the image to be deleted
 */
function _pbj_image_delete_form($form, &$form_state, $node, $fid){

  $image = file_load($fid);

  $form_state['image'] = $image;
  $form_state['node'] = $node;

  $output[] = '<div class="pbj-image-preview">';
  $output[] = theme('image_style', array('style_name'=>'pbj_thumbnail', 'path' => $image->uri));
  $output[] = '</div>';

  $form['#id'] = 'pbj-image-delete';

  $form['image'] = array(
    '#markup' => implode('', $output),
  );

  $form['message'] = array(
    '#markup' => '
      <div class="alert alert-block" style="margin-right:120px;">
      <h4 class="alert-heading">Confirm</h4>
        Are you sure you want to delete this image?
      </div>
    '
  );
  // Submit Button.
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#id' => 'pbj-submit',
    //'#no_button' => TRUE,
    '#ajax' => array(
        'callback' => '_pbj_image_delete_form_callback',
        'wrapper' => 'pbj-image-delete',
        'effect' => 'fade',
      ),
  );
  
  $form['return'] = array(
    '#markup' => l('Cancel', 'pbj/dashboard/'.$node->form_build_id, array('html'=>TRUE, 'attributes'=>array('class'=>array('use-ajax','btn')))),
  );

  return $form;
}

/**
 * Form Builder; Ajax callback
 */
function _pbj_image_delete_form_callback($form, &$form_state) {
  $node = $form_state['node'];
  return _pbj_image_dashboard($node->form_build_id);
}

/**
 * Form Builder; Submit function to delete new image from parent node
 */
function _pbj_image_delete_form_submit($form, &$form_state) {
  $node = $form_state['node'];
  $image = $form_state['image'];
  $field_name = variable_get('pbj_field_' . $node->type, FALSE);
  // Find image
  if(!empty($node->{$field_name}[$node->language])){
    foreach($node->{$field_name}[$node->language] as $delta => $i){
      if($i['fid'] == $image->fid){
        unset($node->{$field_name}[$node->language][$delta]);
        _pbj_save('image', $node);
        file_delete($image);
      }
    }
  }
  drupal_set_message('Image has been deleted.');
}