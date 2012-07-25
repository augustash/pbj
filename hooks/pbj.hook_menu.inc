<?php

/**
 * Implements hook_menu().
 */
function _pbj_menu() {
  $items = array();
  
  $items['admin/config/content/pbj'] = array(
    'title' => t('Peanut Butter & Jelly'),
    'description' => 'WYSIWYG Craziness. Thank you Carl for your inspirational shirt colors!',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('pbj_settings_form'),
    'access arguments' => array('access administration pages'),
    'file' => 'includes/pbj.admin.inc',
    'type' => MENU_NORMAL_ITEM,
  );

  $items['pbj/dashboard/%'] = array(
    'title' => t('Peanut Butter & Jelly'),
    'description' => 'Dashboard',
    'page callback' => '_pbj_image_dashboard',
    'access arguments' => array('access content'),
    'type' => MENU_CALLBACK,
    'page arguments' => array(2),
    'theme callback' => 'ajax_base_page_theme',
    'delivery callback' => 'ajax_deliver',
    'file' => 'includes/pbj.pages.inc',
  );

  // $items['pbj/dashboard/new'] = array(
  //   'title' => t('Peanut Butter & Jelly'),
  //   'description' => 'Dashboard',
  //   'page callback' => '_pbj_image_new',
  //   'access arguments' => array('access content'),
  //   'type' => MENU_CALLBACK,
  //   'theme callback' => 'ajax_base_page_theme',
  //   'delivery callback' => 'ajax_deliver',
  //   'file' => 'includes/pbj.pages.inc',
  // );

  return $items;
}