<?php

/**
* Implements hook_filter_info().
*/
function pbj_filter_info() {
  $filters = array();
  $filters['pbj_markdown_image_classes'] = array(
    'title' => t('Markdown image classes'),
    'description' => t('Adds image class support to markdown filter.'),
    'process callback' => '_pbj_markdown_image_classes_process',
    //'tips callback' => '_pbj_markdown_image_classes_tips',
  );
  return $filters;
}

/**
* Implements filter process callback.
*/
function _pbj_markdown_image_classes_process($text, $filter, $format, $langcode, $cache, $cache_id) {
  $regexp = '/(<img.*)\/>\{([A-Za-z0-9-_\s]*)\}/';
  $replacement = '${1}class="${2}" />';
  return preg_replace($regexp, $replacement, $text);
}

/**
* Implements filter tips callback.
*/
function _pbj_markdown_image_classes_tips($filter, $format, $long = false) {
  // Can be used to explain this input filter if necessary
  if($long) {
    return '';
  }
  else {
    return '';
  }
}